idf_component_register(
    SRCS "fw_info.c"
    INCLUDE_DIRS "."
    REQUIRES esp_system esp_hw_support esp_wifi spi_flash
)