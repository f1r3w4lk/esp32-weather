#ifndef DISPLAY_ASSETS_H
#define DISPLAY_ASSETS_H

#include <stdint.h>

/**
 * @file display_assets.h
 * @brief Font and icon bitmap assets stored in flash memory for OLED display rendering.
 *
 * This file declares bitmap arrays used by the display driver.
 * All assets are stored in flash (ROM) to optimize RAM usage.
 */

/**
 * @defgroup FONT_6X8 Font 6x8
 * @brief Standard monospaced 6x8 font.
 *
 * - Each character uses 6 bytes:
 *   - 5 bytes for glyph width
 *   - 1 byte for horizontal spacing
 *
 * This font is intended for compact text rendering on OLED displays.
 * @{
 */

/**
 * @brief 6x8 pixel bitmap font table.
 *
 * Each entry in the array represents one character's bitmap.
 * Characters follow ASCII index ordering.
 */
extern const uint8_t font6x8[][6];
/** @} */  // end of FONT_6X8 group

/**
 * @defgroup ICONS_24X24 Icons 24x24
 * @brief Weather-related icon bitmaps stored in flash.
 *
 * Each icon is 24x24 pixels → 72 bytes (1 bit per pixel)
 *
 * Icons are indexed by environmental status indicators such as Wi-Fi connectivity and weather.
 * @{
 */

/** @brief Wi-Fi disabled or disconnected icon */
extern const uint8_t icon_wifi_off[72];

/** @brief Wi-Fi connected icon */
extern const uint8_t icon_wifi[72];

/** @brief Sunny weather icon */
extern const uint8_t icon_sun[72];

/** @brief Cloudy weather icon */
extern const uint8_t icon_cloud[72];

/** @brief Rainy weather icon */
extern const uint8_t icon_rain[72];

/** @brief Night / moon weather icon */
extern const uint8_t icon_moon[72];

/** @brief Wind indicator icon */
extern const uint8_t icon_wind[72];

/** @} */  // end of ICONS_24X24 group

#endif  // DISPLAY_ASSETS_H
