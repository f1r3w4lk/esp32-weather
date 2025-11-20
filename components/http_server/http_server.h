#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Start the HTTP server (mounts FS if needed).
 * 
 * Returns ESP_OK on success or esp_err_t on failure.
 */
esp_err_t http_server_start(void);

/** Stop the HTTP server (if running). */
void http_server_stop(void);

#ifdef __cplusplus
}
#endif


#endif  // HTTP_SERVER_H