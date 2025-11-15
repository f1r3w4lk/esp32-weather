#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include "esp_err.h"

esp_err_t nvs_manager_init(void);
esp_err_t nvs_manager_save_str(const char *key, const char *value);
esp_err_t nvs_manager_read_str(const char *key, char *out_value, size_t len);
esp_err_t nvs_manager_erase_all(void);

#endif
