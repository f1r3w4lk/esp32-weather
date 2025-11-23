#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file wifi_manager.h
 * @brief Wi-Fi management module for station mode and configuration mode handling.
 *
 * This module initializes and manages the Wi-Fi functionality of the ESP32.
 * It supports both:
 *  - Normal operation mode: connect to previously stored Wi-Fi credentials.
 *  - Configuration mode: start a local access point for configuration.
 */

/**
 * @brief Initialize Wi-Fi and select operation mode.
 *
 * If valid Wi-Fi credentials are stored, the ESP32 attempts to connect to the AP.
 * If `force_config` is true or connection fails, the device switches to configuration mode:
 * An internal Access Point is created to allow the user to enter new credentials.
 *
 * @param force_config  If true, AP configuration mode is forced regardless of saved credentials.
 */
void wifi_manager_init(bool force_config);

#ifdef __cplusplus
}
#endif

#endif  // WIFI_MANAGER_H
