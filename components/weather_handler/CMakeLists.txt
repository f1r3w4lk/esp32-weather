idf_component_register(
    SRCS "weather_handler.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES http_client json
)