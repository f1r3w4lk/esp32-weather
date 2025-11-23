#ifndef NVS_MANAGER_H
#define NVS_MANAGER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file nvs_manager.h
 * @brief Wrapper for ESP32 Non-Volatile Storage (NVS).
 *
 * Provides simplified helper functions for saving and retrieving
 * configuration parameters stored in NVS.
 */

/**
 * @brief Initialize NVS storage subsystem.
 *
 * Must be called before any NVS read/write operations.
 *
 * @return
 *  - ESP_OK on successful initialization.
 *  - ESP_FAIL or other esp_err_t on failure.
 */
esp_err_t nvs_manager_init(void);

/**
 * @brief Save a string value into NVS.
 *
 * @param key        Null-terminated key identifier.
 * @param value      Null-terminated string to store.
 *
 * @return
 *  - ESP_OK if saved successfully.
 *  - esp_err_t error code on failure.
 */
esp_err_t nvs_manager_save_str(const char *key, const char *value);

/**
 * @brief Read a string from NVS.
 *
 * @param key        Null-terminated key identifier.
 * @param out_value  Output buffer to store retrieved string.
 * @param len        Size of the output buffer.
 *
 * @return
 *  - ESP_OK on success.
 *  - ESP_ERR_NVS_NOT_FOUND if key does not exist.
 *  - esp_err_t on other failures.
 */
esp_err_t nvs_manager_read_str(const char *key, char *out_value, size_t len);

/**
 * @brief Save a double precision floating-point value into NVS.
 *
 * @param key        Null-terminated key identifier.
 * @param value      Floating-point value to store.
 *
 * @return
 *  - ESP_OK on success.
 *  - esp_err_t error code on failure.
 */
esp_err_t nvs_manager_save_double(const char *key, double value);

/**
 * @brief Read a double precision floating-point value from NVS.
 *
 * @param key        Null-terminated key identifier.
 * @param out_value  Pointer to store the retrieved value.
 *
 * @return
 *  - ESP_OK on success.
 *  - ESP_ERR_NVS_NOT_FOUND if key does not exist.
 *  - esp_err_t on other failures.
 */
esp_err_t nvs_manager_read_double(const char *key, double *out_value);

/**
 * @brief Erase all NVS keys in the default namespace.
 *
 * ⚠️ Warning: This removes all stored configuration settings!
 *
 * @return
 *  - ESP_OK on success.
 *  - esp_err_t on failure.
 */
esp_err_t nvs_manager_erase_all(void);

#ifdef __cplusplus
}
#endif

#endif // NVS_MANAGER_H
