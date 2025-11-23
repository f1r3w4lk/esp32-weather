#ifndef HTTP_CLIENT_H
#define HTTP_CLIENT_H

#include "esp_err.h"
#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file http_client.h
 * @brief Generic HTTP request module (GET and POST) for ESP-IDF.
 *
 * This module provides simplified functions to perform HTTP requests
 * and store responses in a user-supplied buffer.
 *
 * HTTPS is supported using ESP-IDF's built-in certificate bundle.
 */

/**
 * @brief Perform an HTTP GET request.
 *
 * @param url              Full request URL (e.g., "https://api.open-meteo.com/v1/...").
 * @param response_buffer  Destination buffer to store the response body.
 * @param max_len          Maximum buffer size available for the response.
 *
 * @return
 *  - ESP_OK on success.
 *  - ESP_ERR_INVALID_ARG for invalid arguments.
 *  - ESP_FAIL for general failure.
 *  - Additional HTTP client error codes may be returned.
 */
esp_err_t http_get(const char *url, char *response_buffer, size_t max_len);

/**
 * @brief Perform an HTTP POST request.
 *
 * @param url              Full request URL (e.g., "https://api.callmebot.com/...").
 * @param post_data        Request body (typically JSON).
 * @param response_buffer  Destination buffer to store the server response.
 * @param max_len          Maximum buffer size available for the response.
 *
 * @return
 *  - ESP_OK on success.
 *  - ESP_ERR_INVALID_ARG for invalid arguments.
 *  - ESP_FAIL for general failure.
 *  - Additional HTTP client error codes may be returned.
 */
esp_err_t http_post(const char *url, const char *post_data, char *response_buffer, size_t max_len);

#ifdef __cplusplus
}
#endif

#endif  // HTTP_CLIENT_H
