#include "http_server.h"
#include "esp_log.h"
#include "nvs_manager.h"
#include "fs_handler.h"
#include "fw_info.h"
#include "cJSON.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>
#include <stdlib.h>

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;

/* ----------------- Helpers ----------------- */

static void send_json_obj(httpd_req_t *req, cJSON *obj)
{
    char *s = cJSON_PrintUnformatted(obj);
    if (!s) {
        httpd_resp_send_500(req);
        cJSON_Delete(obj);
        return;
    }
    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, s);
    free(s);
    cJSON_Delete(obj);
}

/* Read request body into a heap buffer (null-terminated). Caller must free. */
static char *read_request_body(httpd_req_t *req, size_t *out_len)
{
    if (out_len) *out_len = 0;
    int total_len = req->content_len;
    if (total_len <= 0) return NULL;

    char *buf = malloc(total_len + 1);
    if (!buf) return NULL;

    int r = 0;
    int received = 0;
    while (received < total_len) {
        r = httpd_req_recv(req, buf + received, total_len - received);
        if (r <= 0) {
            free(buf);
            return NULL;
        }
        received += r;
    }
    buf[total_len] = '\0';
    if (out_len) *out_len = (size_t)total_len;
    return buf;
}

/* ----------------- API: /api/config (GET) ----------------- */
static esp_err_t api_config_get_handler(httpd_req_t *req)
{
    char ssid[64] = {0};
    char pass[64] = {0};

    nvs_manager_read_str("wifi_ssid", ssid, sizeof(ssid));
    nvs_manager_read_str("wifi_pass", pass, sizeof(pass));

    cJSON *json = cJSON_CreateObject();
    cJSON_AddStringToObject(json, "ssid", ssid);
    cJSON_AddStringToObject(json, "pass", pass );

    send_json_obj(req, json);
    return ESP_OK;
}

/* ----------------- API: /api/config (POST) ----------------- */
static esp_err_t api_config_post_handler(httpd_req_t *req)
{
    size_t body_len = 0;
   char *body = read_request_body(req, &body_len);
   if (!body) {
       httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Empty body or read error");
       return ESP_FAIL;
   }

   cJSON *root = cJSON_Parse(body);
   free(body);
   if (!root) {
       httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Invalid JSON");
       return ESP_FAIL;
   }

   const cJSON *jssid = cJSON_GetObjectItem(root, "wifi_ssid");
   const cJSON *jpass = cJSON_GetObjectItem(root, "wifi_password");
   const cJSON *jlat  = cJSON_GetObjectItem(root, "latitude");
   const cJSON *jlon  = cJSON_GetObjectItem(root, "longitude");

   if (!cJSON_IsString(jssid) || !cJSON_IsString(jpass) ||
       !cJSON_IsNumber(jlat) || !cJSON_IsNumber(jlon)) {

       cJSON_Delete(root);
       httpd_resp_send_err(req, HTTPD_400_BAD_REQUEST, "Missing or invalid fields");
       return ESP_FAIL;
   }

   const char *ssid = jssid->valuestring;
   const char *pass = jpass->valuestring;
   double latitude  = jlat->valuedouble;
   double longitude = jlon->valuedouble;

   ESP_LOGI(TAG, "Saving config: SSID='%s' PASS len=%d, LAT:%f LON:%f",
            ssid, (int)strlen(pass), latitude, longitude);

   // Store in NVS
   nvs_manager_save_str("wifi_ssid", ssid);
   nvs_manager_save_str("wifi_pass", pass);
   nvs_manager_save_double("latitude", latitude);
   nvs_manager_save_double("longitude", longitude);

   cJSON *resp = cJSON_CreateObject();
   cJSON_AddStringToObject(resp, "status", "ok");
   cJSON_AddStringToObject(resp, "msg", "Config saved! Restarting...");
   send_json_obj(req, resp);

   cJSON_Delete(root);

   /* Give client time to get response */
   vTaskDelay(pdMS_TO_TICKS(800));
   esp_restart();
   return ESP_OK;
}

/* ----------------- API: /api/status (GET) ----------------- */
static esp_err_t api_status_get_handler(httpd_req_t *req)
{
    cJSON *root = cJSON_CreateObject();
    if (!root) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    fw_info_t info;
    fw_info_load(&info);

    cJSON_AddStringToObject(root, "fw_version", info.version);
    cJSON_AddStringToObject(root, "fw_build_date", info.build_date);
    cJSON_AddStringToObject(root, "chip_model", info.chip_model);
    cJSON_AddNumberToObject(root, "flash_size_bytes", info.flash_size);
    cJSON_AddNumberToObject(root, "chip_id", info.chip_id);

   char mac_str[18]; // 6 bytes -> "XX:XX:XX:XX:XX:XX"
   snprintf(mac_str, sizeof(mac_str),
             "%02X:%02X:%02X:%02X:%02X:%02X",
             info.mac_addr[0], info.mac_addr[1], info.mac_addr[2],
             info.mac_addr[3], info.mac_addr[4], info.mac_addr[5]);
    cJSON_AddStringToObject(root, "mac_address", mac_str);
    cJSON_AddNumberToObject(root, "free_heap", info.free_heap);

    // Send JSON
    char *s = cJSON_PrintUnformatted(root);
    if (!s) {
        cJSON_Delete(root);
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    httpd_resp_set_type(req, "application/json");
    httpd_resp_sendstr(req, s);

    free(s);
    cJSON_Delete(root);
    return ESP_OK;
}

/* ----------------- File serving from LittleFS ----------------- */
/* Uses fs_handler_read_file() which returns allocated buffer + size */
static esp_err_t file_get_handler(httpd_req_t *req)
{
    char path[512];
    ESP_LOGI(TAG, "HTTP GET request for URI: %s", req->uri);


    if (strcmp(req->uri, "/") == 0) {
        strcpy(path, "/index.html");
    } 
    else {
        strlcpy(path, req->uri, sizeof(path));
        if (strlen(req->uri) >= sizeof(path)) {
            ESP_LOGW(TAG, "URI too long, truncated: %s", req->uri);
        }
    }

    // Remove query string
    char *q = strchr(path, '?');
    if (q) *q = '\0';

    // Block directory traversal attempts
    if (strstr(path, "..")) {
        ESP_LOGW(TAG, "Blocked path traversal: %s", path);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    /* Determine MIME */
    if (strstr(path, ".css")) httpd_resp_set_type(req, "text/css");
    else if (strstr(path, ".js")) httpd_resp_set_type(req, "application/javascript");
    else if (strstr(path, ".png")) httpd_resp_set_type(req, "image/png");
    else httpd_resp_set_type(req, "text/html");

    char *buf = NULL;
    size_t size = 0;
    esp_err_t r = fs_handler_read_file(path, &buf, &size);
    if (r != ESP_OK || !buf) {
        ESP_LOGW(TAG, "File not found: %s", path);
        httpd_resp_send_404(req);
        return ESP_FAIL;
    }

    httpd_resp_send(req, buf, size);
    free(buf);
    return ESP_OK;
}

static esp_err_t root_get_handler(httpd_req_t *req)
{
    httpd_resp_set_type(req, "text/html");
    return file_get_handler(req);
}

/* ----------------- URI mappings ----------------- */
static const httpd_uri_t uri_api_status = {
    .uri = "/api/status",
    .method = HTTP_GET,
    .handler = api_status_get_handler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_api_get = {
    .uri = "/api/config",
    .method = HTTP_GET,
    .handler = api_config_get_handler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_api_post = {
    .uri = "/api/config",
    .method = HTTP_POST,
    .handler = api_config_post_handler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_files = {
    .uri = "/*",
    .method = HTTP_GET,
    .handler = file_get_handler,
    .user_ctx = NULL
};

static const httpd_uri_t uri_root = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
    .user_ctx = NULL
};

/* ----------------- Server start / stop ----------------- */
esp_err_t http_server_start(void)
{
    if (server != NULL) {
        ESP_LOGW(TAG, "HTTP server already running");
        return ESP_OK;
    }

    /* ensure FS is mounted (safe to call multiple times) */
    esp_err_t fsret = fs_handler_init();
    if (fsret != ESP_OK) {
        ESP_LOGW(TAG, "fs_handler_init returned %s; continuing (files might be missing)",
                 esp_err_to_name(fsret));
        /* We continue so that config endpoints still work even if FS empty.
           If you prefer to fail, return fsret here. */
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    /* slightly larger recv timeout for bigger POSTs if needed */
    config.recv_wait_timeout = 2000;

    /* Instruct the server to use built-in wildcard matching logic */
    config.uri_match_fn = httpd_uri_match_wildcard;

    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed: %s", esp_err_to_name(err));
        return err;
    }

    /* Register API + file handlers */
    esp_err_t r = httpd_register_uri_handler(server, &uri_root);
    ESP_LOGI(TAG, "Register / returned: %s", esp_err_to_name(r));
    r = httpd_register_uri_handler(server, &uri_api_status);
    ESP_LOGI(TAG, "Register /api/status GET returned: %s", esp_err_to_name(r));
    r = httpd_register_uri_handler(server, &uri_api_get);
    ESP_LOGI(TAG, "Register /api/config GET returned: %s", esp_err_to_name(r));
    r = httpd_register_uri_handler(server, &uri_api_post);
    ESP_LOGI(TAG, "Register /api/config POST returned: %s", esp_err_to_name(r));
    r = httpd_register_uri_handler(server, &uri_files);
    ESP_LOGI(TAG, "Register /* returned: %s", esp_err_to_name(r));

    ESP_LOGI(TAG, "HTTP server started");
    return ESP_OK;
}

void http_server_stop(void)
{
    if (server) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "HTTP server stopped");
    }
}
