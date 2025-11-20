idf_component_register(
    SRCS "http_server.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_http_server json nvs_flash
    REQUIRES nvs_manager fs_handler fw_info
)
