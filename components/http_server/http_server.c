#include "http_server.h"
#include "esp_log.h"
#include "nvs_manager.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include <string.h>

static const char *TAG = "HTTP_SERVER";
static httpd_handle_t server = NULL;

/* --- Handlers --- */
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
    int len = httpd_req_recv(req, buf, sizeof(buf)-1);
    if (len <= 0) {
        httpd_resp_send_500(req);
        return ESP_FAIL;
    }

    buf[len] = '\0';

    char ssid[64] = {0};
    char pass[64] = {0};

    sscanf(buf, "ssid=%63[^&]&pass=%63s", ssid, pass);

    ESP_LOGI(TAG, "Saving credentials: SSID='%s'", ssid);

    nvs_manager_save_str("wifi_ssid", ssid);
    nvs_manager_save_str("wifi_pass", pass);

    httpd_resp_sendstr(req, "Credenciais salvas. Reiniciando...");
    vTaskDelay(1500 / portTICK_PERIOD_MS);
    esp_restart();
    return ESP_OK;
}

/* --- URI entries --- */
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

/* --- Start server ---- */
esp_err_t http_server_start(void)
{
    if (server != NULL) {
        ESP_LOGW(TAG, "Server already running");
        return ESP_OK;
    }

    httpd_config_t config = HTTPD_DEFAULT_CONFIG();

    esp_err_t err = httpd_start(&server, &config);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "httpd_start failed: %s", esp_err_to_name(err));
        return err;
    }

    httpd_register_uri_handler(server, &root_get);
    httpd_register_uri_handler(server, &config_post);

    ESP_LOGI(TAG, "HTTP config server started");
    return ESP_OK;
}

/* --- Stop server --- */
void http_server_stop(void)
{
    if (server) {
        httpd_stop(server);
        server = NULL;
        ESP_LOGI(TAG, "HTTP server stopped");
    }
}

/* Optional helper */
esp_err_t http_server_send_restart(httpd_req_t *req)
{
    httpd_resp_sendstr(req, "Reiniciando...");
    vTaskDelay(pdMS_TO_TICKS(1000));
    esp_restart();
    return ESP_OK;
}
