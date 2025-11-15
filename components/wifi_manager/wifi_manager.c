#include "wifi_manager.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "esp_netif.h"
#include "nvs_manager.h"
#include "esp_http_server.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "WIFI_MANAGER";
static httpd_handle_t server = NULL;
static bool wifi_initialized = false;

/* --- Forward declarations --- */
static void base_init_once(void);
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data);
static void wifi_init_sta(const char *ssid, const char *pass);
static void wifi_init_softap(void);
static void start_config_server(void);

/* --- HTTP handlers (keeps simple form POST) --- */
static esp_err_t root_get_handler(httpd_req_t *req)
{
    const char *html =
        "<!DOCTYPE html><html><body>"
        "<h2>ESP Wi-Fi Config</h2>"
        "<form action=\"/config\" method=\"post\">"
        "SSID:<br><input name=\"ssid\"><br>"
        "Password:<br><input name=\"pass\" type=\"password\"><br><br>"
        "<input type=\"submit\" value=\"Salvar\">"
        "</form></body></html>";
    httpd_resp_send(req, html, HTTPD_RESP_USE_STRLEN);
    return ESP_OK;
}

static esp_err_t config_post_handler(httpd_req_t *req)
{
    char buf[256];
    int ret = httpd_req_recv(req, buf, sizeof(buf) - 1);
    if (ret <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }
    buf[ret] = '\0';

    char ssid[64] = {0}, pass[64] = {0};
    /* form-urlencoded payload */
    sscanf(buf, "ssid=%63[^&]&pass=%63s", ssid, pass);

    ESP_LOGI(TAG, "Received SSID='%s' PASS len=%d", ssid, (int)strlen(pass));
    nvs_manager_save_str("wifi_ssid", ssid);
    nvs_manager_save_str("wifi_pass", pass);

    httpd_resp_sendstr(req, "Credenciais salvas. Reiniciando...");
    vTaskDelay(pdMS_TO_TICKS(1500));
    esp_restart();
    return ESP_OK;
}

static const httpd_uri_t root_get = {
    .uri = "/",
    .method = HTTP_GET,
    .handler = root_get_handler,
};

static const httpd_uri_t config_post = {
    .uri = "/config",
    .method = HTTP_POST,
    .handler = config_post_handler,
};

/* --- base init (run once) --- */
static void base_init_once(void)
{
    if (wifi_initialized) return;

    ESP_ERROR_CHECK(esp_netif_init());
    ESP_ERROR_CHECK(esp_event_loop_create_default());

    wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
    ESP_ERROR_CHECK(esp_wifi_init(&cfg));

    wifi_initialized = true;
    ESP_LOGI(TAG, "Base Wi-Fi subsystem initialized");
}

/* --- start config HTTP server --- */
static void start_config_server(void)
{
    httpd_config_t config = HTTPD_DEFAULT_CONFIG();
    if (httpd_start(&server, &config) == ESP_OK) {
        httpd_register_uri_handler(server, &root_get);
        httpd_register_uri_handler(server, &config_post);
        ESP_LOGI(TAG, "HTTP config server started");
    } else {
        ESP_LOGE(TAG, "Failed to start config HTTP server");
    }
}

/* --- Wi-Fi event handler --- */
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START) {
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED) {
        ESP_LOGW(TAG, "STA disconnected, attempting reconnection...");
        esp_wifi_connect();
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ESP_LOGI(TAG, "STA got IP");
    }
}

/* --- Wi-Fi STA init (assume base_init_once already called) --- */
static void wifi_init_sta(const char *ssid, const char *pass)
{
    ESP_LOGI(TAG, "Starting STA mode to connect to SSID: %s", ssid);

    /* create default netif for STA */
    esp_netif_create_default_wifi_sta();

    esp_event_handler_instance_t instance_any_id;
    esp_event_handler_instance_t instance_got_ip;
    ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                        ESP_EVENT_ANY_ID,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_any_id));
    ESP_ERROR_CHECK(esp_event_handler_instance_register(IP_EVENT,
                                                        IP_EVENT_STA_GOT_IP,
                                                        &wifi_event_handler,
                                                        NULL,
                                                        &instance_got_ip));

    wifi_config_t wifi_config = { 0 };
    strncpy((char *)wifi_config.sta.ssid, ssid, sizeof(wifi_config.sta.ssid)-1);
    strncpy((char *)wifi_config.sta.password, pass, sizeof(wifi_config.sta.password)-1);

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_STA));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_STA, &wifi_config));
    ESP_ERROR_CHECK(esp_wifi_start());
}

/* --- Wi-Fi SoftAP init (assume base_init_once already called) --- */
static void wifi_init_softap(void)
{
    ESP_LOGW(TAG, "Starting SoftAP for config mode...");

    /* create default netif for AP after esp_wifi_init (base_init_once does that) */
    esp_netif_create_default_wifi_ap();

    wifi_config_t ap_config = {
        .ap = {
            .ssid = "ESP_Config",
            .ssid_len = 0,
            .channel = 1,
            .max_connection = 4,
            .authmode = WIFI_AUTH_WPA_WPA2_PSK,
        },
    };
    const char *ap_pass = "12345678";
    strncpy((char *)ap_config.ap.password, ap_pass, sizeof(ap_config.ap.password)-1);

    if (strlen(ap_pass) == 0) {
        ap_config.ap.authmode = WIFI_AUTH_OPEN;
    }

    ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
    ESP_ERROR_CHECK(esp_wifi_set_config(WIFI_IF_AP, &ap_config));
    ESP_ERROR_CHECK(esp_wifi_start());

    /* start HTTP server so user can POST credentials */
    start_config_server();

    while (true)
    {
        ESP_LOGI(TAG, "Waiting new configuration!");
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

/* --- Public init --- */
void wifi_manager_init(bool force_config)
{
    /* ensure NVS is initialized (nvs_manager_init internally call nvs_flash_init) */
    nvs_manager_init();

    /* initialize base Wi-Fi internals ONCE and in the correct order */
    base_init_once();

    /* cfg button pressed → ignorate NVS and start AP */
    if (force_config) {
        ESP_LOGW(TAG, "Force config mode requested → Starting SoftAP");
        wifi_init_softap();
        return;
    }

    /* Normal case – attempt to read credentials */
    char ssid[64] = {0}, pass[64] = {0};
    esp_err_t r1 = nvs_manager_read_str("wifi_ssid", ssid, sizeof(ssid));
    esp_err_t r2 = nvs_manager_read_str("wifi_pass", pass, sizeof(pass));

    if (r1 == ESP_OK && r2 == ESP_OK && strlen(ssid) > 0) {
        ESP_LOGI(TAG, "Stored credentials found, attempting STA connect");
        wifi_init_sta(ssid, pass);
    } else {
        ESP_LOGW(TAG, "No stored credentials → Starting SoftAP");
        wifi_init_softap();
    }
}
