#include <string.h>
#include "esp_log.h"
#include "esp_crt_bundle.h"
#include "esp_http_client.h"
#include "esp_err.h"

static const char *TAG = "HTTP_CLIENT";

/**
 * @brief Help struct to store the HTTP requests state
 */
typedef struct {
    char *buffer;
    size_t max_len;
    size_t len;
} http_response_ctx_t;

/**
 * @brief Generic handler of HTTP (GET/POST) events
 */
static esp_err_t _http_event_handler(esp_http_client_event_t *evt)
{
    http_response_ctx_t *ctx = (http_response_ctx_t *)evt->user_data;

    switch (evt->event_id) {
        case HTTP_EVENT_ERROR:
            ESP_LOGD(TAG, "HTTP_EVENT_ERROR");
            break;

        case HTTP_EVENT_ON_CONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_CONNECTED");
            if (ctx)
                ctx->len = 0;
            break;

        case HTTP_EVENT_ON_DATA:
            if (ctx && evt->data && evt->data_len > 0) {
                size_t copy_len = evt->data_len;

                if (ctx->len + copy_len >= ctx->max_len) {
                    copy_len = ctx->max_len - ctx->len - 1;
                    ESP_LOGW(TAG, "Response buffer truncated (max=%d)", (int)ctx->max_len);
                }

                memcpy(ctx->buffer + ctx->len, evt->data, copy_len);
                ctx->len += copy_len;
                ctx->buffer[ctx->len] = '\0';
            }
            break;

        case HTTP_EVENT_ON_FINISH:
            ESP_LOGD(TAG, "HTTP_EVENT_ON_FINISH (len=%d)", (int)(ctx ? ctx->len : 0));
            break;

        case HTTP_EVENT_DISCONNECTED:
            ESP_LOGD(TAG, "HTTP_EVENT_DISCONNECTED");
            break;

        default:
            break;
    }

    return ESP_OK;
}

/**
 * @brief Execute one HTTP GET request
 */
esp_err_t http_get(const char *url, char *response_buffer, size_t max_len)
{
    if (!url || !response_buffer || max_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    http_response_ctx_t ctx = { .buffer = response_buffer, .max_len = max_len, .len = 0 };

    ESP_LOGI(TAG, "HTTP GET: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = &ctx,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 10000,
        .skip_cert_common_name_check = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP GET Status = %d", status);
    } else {
        ESP_LOGE(TAG, "HTTP GET request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}

/**
 * @brief Execute one HTTP POST request
 */
esp_err_t http_post(const char *url, const char *post_data, char *response_buffer, size_t max_len)
{
    if (!url || !post_data || !response_buffer || max_len == 0) {
        return ESP_ERR_INVALID_ARG;
    }

    http_response_ctx_t ctx = { .buffer = response_buffer, .max_len = max_len, .len = 0 };

    ESP_LOGI(TAG, "HTTP POST: %s", url);

    esp_http_client_config_t config = {
        .url = url,
        .event_handler = _http_event_handler,
        .user_data = &ctx,
        .crt_bundle_attach = esp_crt_bundle_attach,
        .timeout_ms = 10000,
        .skip_cert_common_name_check = true,
    };

    esp_http_client_handle_t client = esp_http_client_init(&config);
    if (!client) {
        ESP_LOGE(TAG, "Failed to initialize HTTP client");
        return ESP_FAIL;
    }

    esp_http_client_set_method(client, HTTP_METHOD_POST);
    esp_http_client_set_header(client, "Content-Type", "application/json");
    esp_http_client_set_post_field(client, post_data, strlen(post_data));

    esp_err_t err = esp_http_client_perform(client);
    if (err == ESP_OK) {
        int status = esp_http_client_get_status_code(client);
        ESP_LOGI(TAG, "HTTP POST Status = %d", status);
    } else {
        ESP_LOGE(TAG, "HTTP POST request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    return err;
}
