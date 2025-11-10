#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "weather_handler.h"
#include "display_manager.h"
#include "display_assets.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "==== ESP32 Weather Display v2 ====");

    /* Display Initialization */
    display_init();

    display_draw_icon(0, 4, 24, 24, icon_wifi_off);
    display_draw_text_6x8(40, 5, "Connecting");
    display_draw_text_6x8(58, 18, "WiFi");
    display_refresh();

    // Start the WiFi Connection
    wifi_manager_init();

    // Wait WiFi Connection before follow the next step (the best approach is substitute by event callback)
    vTaskDelay(pdMS_TO_TICKS(10000));

    display_clear();
    display_draw_icon(0, 4, 24, 24, icon_wifi);
    display_draw_text_6x8(58, 5, "Wifi");
    display_draw_text_6x8(41, 18, "Connected");
    display_refresh();

    // Struct to store weather data
    weather_data_t weather;

    while (true) {
        ESP_LOGI(TAG, "📡 Fetching weather data...");

        // Get open-meteo data
        if (weather_data_fetch(-30.0133836, -51.1459955, &weather) == ESP_OK) {
            ESP_LOGI(
                TAG,
                "🌡️ Temp: %.1f°C | 💧 Humidity: %.0f%% | Precipitation %.2fmm | %s | %s",
                weather.temperature, weather.humidity, weather.precipitation,
                weather_data_wmo_description(weather.weather_code),
                weather.is_day ? "Day" : "Night");

            char line[32];
            display_clear();

            // Display Weather Icon
            if (strcmp(weather_data_wmo_description(weather.weather_code), "Clear") == 0) {
                display_draw_icon(0, 4, 24, 24, (weather.is_day ? icon_sun : icon_moon));
            } else if (strcmp(weather_data_wmo_description(weather.weather_code), "Cloudy") == 0) {
                display_draw_icon(0, 4, 24, 24, icon_cloud);
            } else if (strcmp(weather_data_wmo_description(weather.weather_code), "Rain") == 0) {
                display_draw_icon(0, 4, 24, 24, icon_rain);
            } else {
                display_draw_text_6x8(0, 12, "???");
            }

            snprintf(line, sizeof(line), "T:%.1fC", weather.temperature);
            display_draw_text_12x16(33, 0, line);
            snprintf(line, sizeof(line), "H:%.0f%%", weather.humidity);
            display_draw_text_6x8(33, 20, line);
            display_refresh();
        } else {
            ESP_LOGE(TAG, "❌ Failed to fetch weather data");
            display_clear();
            display_draw_text_6x8(0, 0, "Weather fetch fail");
            display_refresh();
        }

        // Update every 10 minutes
        vTaskDelay(pdMS_TO_TICKS(600000));
    }
}
