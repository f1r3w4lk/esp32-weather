idf_component_register(
    SRCS "wifi_manager.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_event esp_wifi esp_netif esp_http_server nvs_flash json
    REQUIRES nvs_manager
)
