idf_component_register(
    SRCS "fs_handler.c"
    INCLUDE_DIRS "."
    PRIV_REQUIRES esp_system vfs joltwallet__littlefs
)

# Add LittleFS partition build from folder littlefs_data
littlefs_create_partition_image(storage littlefs_data FLASH_IN_PROJECT)
