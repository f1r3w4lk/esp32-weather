#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_err.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file http_server.h
 * @brief Embedded Web Server for configuration and API handling.
 *
 * This module initializes and manages the internal HTTP server
 * responsible for serving configuration pages and REST API endpoints.
 */

/**
 * @brief Start the HTTP server.
 *
 * Automatically mounts and verifies filesystem availability when required.
 *
 * @return
 *  - ESP_OK on success.
 *  - esp_err_t error code on failure.
 */
esp_err_t http_server_start(void);

/**
 * @brief Stop the HTTP server if currently running.
 *
 * Releases all resources allocated to the server instance.
 */
void http_server_stop(void);

#ifdef __cplusplus
}
#endif

#endif  // HTTP_SERVER_H
