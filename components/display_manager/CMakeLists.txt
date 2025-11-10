idf_component_register(
    SRCS "display_manager.c" "display_assets.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES driver esp_lcd esp_timer
)