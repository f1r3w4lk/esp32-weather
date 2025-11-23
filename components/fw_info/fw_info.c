#include "fw_info.h"
#include "esp_system.h"
#include "esp_chip_info.h"
#include "esp_mac.h"
#include "esp_heap_caps.h"
#include "esp_flash.h"

#include <string.h>

// FW Version
#define FW_VERSION "1.0.0"

void fw_info_load(fw_info_t *info)
{
    esp_chip_info_t chip;
    esp_chip_info(&chip);

    uint8_t mac[6];
    esp_read_mac(mac, ESP_MAC_WIFI_STA);

    info->version = FW_VERSION;
    info->build_date = __DATE__ " " __TIME__;

    /* To others versions of ESP32, implement it! */
    info->chip_model = (chip.model == CHIP_ESP32) ? "ESP32" : "Unknown";

    esp_flash_get_size(NULL, &(info->flash_size));
    info->chip_id = ((uint32_t)mac[3] << 16) | ((uint32_t)mac[4] << 8) | (uint32_t)mac[5];

    memcpy(info->mac_addr, mac, 6);
    info->free_heap = esp_get_free_heap_size();
}
