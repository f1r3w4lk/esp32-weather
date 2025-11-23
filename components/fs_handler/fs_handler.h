#ifndef FS_HANDLER_H
#define FS_HANDLER_H

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file fs_handler.h
 * @brief LittleFS file system abstraction for ESP32 firmware.
 *
 * This module provides a simplified interface for common file operations
 * such as reading, writing, deleting, and checking file existence on the
 * LittleFS partition configured in ESP-IDF.
 */

/**
 * @brief Initialize LittleFS and mount the configured partition.
 *
 * This function must be called before performing any other filesystem operations.
 *
 * @return ESP_OK on success, otherwise an error code.
 */
esp_err_t fs_handler_init(void);

/**
 * @brief Read an entire file into a dynamically allocated buffer.
 *
 * The caller becomes responsible for freeing the returned buffer via `free()`.
 *
 * @param path File path inside LittleFS (e.g., "/index.html").
 * @param[out] out_buf Pointer to receive dynamically allocated buffer with file data.
 * @param[out] out_len Pointer to receive file size in bytes.
 *
 * @return ESP_OK on successful read, ESP_FAIL on error or file not found.
 */
esp_err_t fs_handler_read_file(const char *path, char **out_buf, size_t *out_len);

/**
 * @brief Write a buffer into a file, replacing its contents if it exists.
 *
 * @param path File path inside LittleFS.
 * @param data Pointer to the data buffer to write.
 * @param len Number of bytes to write.
 *
 * @return ESP_OK on success, otherwise an error code.
 */
esp_err_t fs_handler_write_file(const char *path, const char *data, size_t len);

/**
 * @brief Delete a file if it exists.
 *
 * @param path File path inside LittleFS.
 * @return ESP_OK if deleted or file did not exist, otherwise an error code.
 */
esp_err_t fs_handler_delete_file(const char *path);

/**
 * @brief Check whether a file exists in LittleFS.
 *
 * @param path File path inside LittleFS.
 * @return true if the file exists, false otherwise.
 */
bool fs_handler_exists(const char *path);

/**
 * @brief List all files located in the filesystem root.
 *
 * Prints file information using ESP-IDF logging.
 */
void fs_handler_list_files(void);

#ifdef __cplusplus
}
#endif

#endif  // FS_HANDLER_H
