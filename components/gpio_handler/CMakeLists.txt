idf_component_register(
    SRCS "gpio_handler.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_driver_gpio driver
)