/**
 * @file main.c
 * @brief Main application entry point for ESP32 Weather Display v2.
 *
 * This module initializes hardware interfaces, connects to Wi-Fi,
 * retrieves configuration parameters, periodically fetches weather data
 * from the Open-Meteo API and displays it on the OLED screen.
 */

#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "wifi_manager.h"
#include "weather_handler.h"
#include "display_manager.h"
#include "display_assets.h"
#include "gpio_handler.h"
#include "nvs_manager.h"

#define DEFAULT_LATITUDE -30.0133836
#define DEFAULT_LONGITUDE -51.1459955

static const char *TAG = "MAIN";

/**
 * @brief Main application logic (FreeRTOS entry point).
 *
 * Responsibilities:
 *  - Initialize GPIO controller for button reading.
 *  - Initialize display and show Wi-Fi connection status.
 *  - Connect to Wi-Fi (or enter configuration AP mode).
 *  - Load saved latitude/longitude from NVS (fallback to defaults).
 *  - Periodically fetch weather data from Open-Meteo API.
 *  - Decode weather response and update OLED rendering.
 *
 * The task runs indefinitely, polling weather data every 10 minutes.
 */
void app_main(void)
{
    ESP_LOGI(TAG, "==== ESP32 Weather Display v2 ====");

    // GPIO Handler Initialization
    gpio_handler_init();

    /* Display Initialization */
    display_init();

    // Show Icon and Text of WiFi Connecting
    display_show_wifi_connecting();

    // Start the WiFi Connection, check if button pressed if yes, enter in config mode
    wifi_manager_init(gpio_handler_is_config_button_pressed());

    // Wait WiFi Connection before follow the next step
    vTaskDelay(pdMS_TO_TICKS(10000));

    // Show Icon and Text of WiFi Connected
    display_show_wifi_connected();

    // Initialize the nvs_manager
    nvs_manager_init();

    /* Get saved lat long from NVS */
    double latitude = 0.0, longitude = 0.0;
    if (nvs_manager_read_double("latitude", &latitude) != ESP_OK) {
        latitude = DEFAULT_LATITUDE;
    }

    if (nvs_manager_read_double("longitude", &longitude) != ESP_OK) {
        longitude = DEFAULT_LONGITUDE;
    }

    // Struct to store weather data
    weather_data_t weather;

    while (true) {
        ESP_LOGI(TAG, "📡 Fetching weather data...");

        // Get open-meteo data
        if (weather_data_fetch((float)latitude, (float)longitude, &weather) == ESP_OK) {
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
