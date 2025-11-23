#ifndef FW_INFO_H
#define FW_INFO_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file fw_info.h
 * @brief Firmware information service for system and device metadata.
 *
 * This module provides runtime access to firmware versioning details,
 * hardware identifiers, and memory usage statistics. It is useful for
 * web configuration panels, remote diagnostics, or system status reporting.
 */

/**
 * @struct fw_info_t
 * @brief Firmware and device runtime information.
 *
 * This structure is populated by ::fw_info_load() to report key
 * hardware and firmware attributes from the ESP32 environment.
 *
 * @var fw_info_t::version
 *     Null-terminated string that represents the firmware version.
 *
 * @var fw_info_t::build_date
 *     Build timestamp string, typically generated via compiler macros
 *     such as `__DATE__` and `__TIME__`.
 *
 * @var fw_info_t::chip_model
 *     Human-readable string describing the ESP32 chip variant.
 *
 * @var fw_info_t::flash_size
 *     Size of the onboard flash memory in bytes.
 *
 * @var fw_info_t::mac_addr
 *     6-byte device MAC address (station interface).
 *
 * @var fw_info_t::chip_id
 *     Unique identifier derived from chip eFuse values.
 *
 * @var fw_info_t::free_heap
 *     Current free heap memory available in bytes.
 */
typedef struct {
    const char *version;
    const char *build_date;
    const char *chip_model;
    uint32_t flash_size;
    uint8_t mac_addr[6];
    uint32_t chip_id;
    size_t free_heap;
} fw_info_t;

/**
 * @brief Load firmware and hardware information into a @ref fw_info_t structure.
 *
 * Populates every field of the provided structure by querying ESP-IDF APIs.
 *
 * @param[out] info Pointer to structure to be populated.
 */
void fw_info_load(fw_info_t *info);

#ifdef __cplusplus
}
#endif

#endif  // FW_INFO_H
