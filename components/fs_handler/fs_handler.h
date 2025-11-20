#ifndef FS_HANDLER_H
#define FS_HANDLER_H

#include "esp_err.h"
#include <stdbool.h>
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Initialize the LittleFS and mount the configured partition */
esp_err_t fs_handler_init(void);

/** Read a entire file to a buffer dinamically allocated 
 * The caller should free using free()
 */
esp_err_t fs_handler_read_file(const char *path, char **out_buf, size_t *out_len);

/** Write data in a file (overwrite) */
esp_err_t fs_handler_write_file(const char *path, const char *data, size_t len);

/** Delete a file */
esp_err_t fs_handler_delete_file(const char *path);

/** Verify if a file exists */
bool fs_handler_exists(const char *path);

/** List files in root */
void fs_handler_list_files(void);

#ifdef __cplusplus
}
#endif

#endif  // FS_HANDLER_H