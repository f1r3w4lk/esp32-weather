#ifndef FW_INFO_H
#define FW_INFO_H

#include <stdint.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
    const char *version;
    const char *build_date;
    const char *chip_model;
    uint32_t flash_size;
    uint8_t mac_addr[6];
    uint32_t chip_id;
    size_t free_heap;
} fw_info_t;

void fw_info_load(fw_info_t *info);


#ifdef __cplusplus
}
#endif

#endif  // FW_INFO_H