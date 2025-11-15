#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "weather_handler.h"
#include "display_manager.h"
#include "display_assets.h"
#include "gpio_handler.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "==== ESP32 Weather Display v2 ====");

    // Gpio Handler Initialization
    gpio_handler_init();

    /* Display Initialization */
    display_init();

    // Show Icon and Text of WiFi Connected
    display_show_wifi_connecting();

    // Start the WiFi Connection, check if button pressed if yes, enter in config mode
    wifi_manager_init(gpio_handler_is_config_button_pressed());

    // Wait WiFi Connection before follow the next step (the best approach is substitute by event callback)
    vTaskDelay(pdMS_TO_TICKS(10000));

    // Show Icon and Text of WiFi Connected
    display_show_wifi_connected();
    
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
