#ifndef HTTP_SERVER_H
#define HTTP_SERVER_H

#include "esp_http_server.h"

#ifdef __cplusplus
extern "C" {
#endif

/** Start HTTP config server (AP mode) */
esp_err_t http_server_start(void);

/** Stop HTTP server (if running) */
void http_server_stop(void);

/** Exposed handlers for Wi-Fi manager */
esp_err_t http_server_send_restart(httpd_req_t *req);

#ifdef __cplusplus
}
#endif

#endif  // HTTP_SERVER_H