idf_component_register(
    SRCS "http_server.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_http_server nvs_flash json
    REQUIRES nvs_manager
)
