idf_component_register(
    SRCS "esp32_weather_display_v2.c"
    INCLUDE_DIRS "."
    REQUIRES wifi_manager http_client weather_handler display_manager gpio_handler nvs_manager
)