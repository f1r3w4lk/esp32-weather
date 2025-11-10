#include <stdio.h>
#include <string.h>
#include "esp_log.h"
#include "cJSON.h"
#include "http_client.h"
#include "weather_handler.h"

static const char *TAG = "WEATHER_DATA";

// Buffer para resposta HTTP (4 KB é suficiente para Open-Meteo)
#define WEATHER_HTTP_BUFFER_SIZE 4096
static char http_response[WEATHER_HTTP_BUFFER_SIZE];

/**
 * @brief Constrói a URL da API Open-Meteo.
 */
static void build_weather_url(char *url_out, size_t max_len, float lat, float lon)
{
    snprintf(url_out, max_len,
             "https://api.open-meteo.com/v1/forecast?"
             "latitude=%.6f&longitude=%.6f&current=temperature_2m,relative_humidity_2m,"
             "is_day,precipitation,weather_code&forecast_days=1",
             lat, lon);
}

/**
 * @brief Realiza o parsing do JSON retornado pela API.
 */
static esp_err_t parse_weather_json(const char *json, weather_data_t *out)
{
    cJSON *root = cJSON_Parse(json);
    if (!root) {
        ESP_LOGE(TAG, "Failed to parse JSON");
        return ESP_FAIL;
    }

    cJSON *current = cJSON_GetObjectItem(root, "current");
    if (!current) {
        ESP_LOGE(TAG, "Missing 'current' field in JSON");
        cJSON_Delete(root);
        return ESP_FAIL;
    }

    out->temperature = cJSON_GetObjectItem(current, "temperature_2m")->valuedouble;
    out->humidity = cJSON_GetObjectItem(current, "relative_humidity_2m")->valuedouble;
    out->precipitation = cJSON_GetObjectItem(current, "precipitation")->valuedouble;
    out->weather_code = cJSON_GetObjectItem(current, "weather_code")->valueint;
    out->is_day = cJSON_GetObjectItem(current, "is_day")->valueint;

    cJSON_Delete(root);
    return ESP_OK;
}

/**
 * @brief Executa a requisição HTTP e faz o parsing dos dados.
 */
esp_err_t weather_data_fetch(float latitude, float longitude, weather_data_t *out_data)
{
    if (!out_data) {
        return ESP_ERR_INVALID_ARG;
    }

    char url[256];
    build_weather_url(url, sizeof(url), latitude, longitude);

    ESP_LOGI(TAG, "Fetching weather data from: %s", url);

    esp_err_t err = http_get(url, http_response, sizeof(http_response));
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "HTTP GET failed: %s", esp_err_to_name(err));
        return err;
    }

    err = parse_weather_json(http_response, out_data);
    if (err != ESP_OK) {
        ESP_LOGE(TAG, "Failed to parse weather data");
        return err;
    }

    ESP_LOGI(TAG, "Parsed weather data: T=%.1f°C, RH=%.0f%%, P=%.2fmm, WMO=%d, Day=%d",
             out_data->temperature, out_data->humidity, out_data->precipitation,
             out_data->weather_code, out_data->is_day);

    return ESP_OK;
}

/**
 * @brief Conversão simples de código WMO → descrição textual.
 */
const char *weather_data_wmo_description(int code)
{
    switch (code) {
        case 0:
            return "Clear";
        case 1:
        case 2:
        case 3:
            return "Cloudy";
        case 45:
        case 48:
            return "Fog";
        case 51:
        case 53:
        case 55:
            return "Drizzle";
        case 61:
        case 63:
        case 65:
            return "Rain";
        case 71:
        case 73:
        case 75:
            return "Snow";
        case 80:
        case 81:
        case 82:
            return "Showers";
        case 95:
            return "Thunderstorm";
        default:
            return "Unknown";
    }
}
