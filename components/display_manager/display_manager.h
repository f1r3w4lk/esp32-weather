#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @file display_manager.h
 * @brief Display control and rendering API for the ESP32 OLED UI.
 *
 * This module provides initialization and drawing utilities to control
 * a monochrome pixel-based OLED display, including text rendering using
 * custom fonts and predefined UI messages (e.g., Wi-Fi connection status).
 */

/**
 * @brief Initialize the display hardware and clear screen.
 *
 * This function must be called before any drawing operations.
 *
 * @return ESP_OK on success, otherwise an error code.
 */
esp_err_t display_init(void);

/**
 * @brief Clear the display buffer (does not immediately update the screen).
 */
void display_clear(void);

/**
 * @brief Refresh the screen by sending the current buffer over I2C/SPI.
 */
void display_refresh(void);

/**
 * @name Drawing primitives
 * @{
 */

/**
 * @brief Draw a single pixel at a specified location.
 *
 * @param x Horizontal coordinate in pixels.
 * @param y Vertical coordinate in pixels.
 * @param on True to light the pixel, false to turn it off.
 */
void display_draw_pixel(int x, int y, bool on);

/**
 * @brief Draw a bitmap icon to the display buffer.
 *
 * @param x Top-left X coordinate.
 * @param y Top-left Y coordinate.
 * @param w Icon width in pixels.
 * @param h Icon height in pixels.
 * @param bitmap Pointer to bitmap pixel data (1bpp).
 */
void display_draw_icon(int x, int y, int w, int h, const uint8_t *bitmap);
/** @} */  // end primitives

/**
 * @name Text rendering functions
 * @{
 */

/**
 * @brief Draw a single 6x8 font character.
 *
 * @param x Top-left X position.
 * @param y Top-left Y position.
 * @param c ASCII character to draw.
 */
void display_draw_char_6x8(int x, int y, char c);

/**
 * @brief Draw a null-terminated string using 6x8 font.
 *
 * @param x Starting X position.
 * @param y Starting Y position.
 * @param text Pointer to C string.
 */
void display_draw_text_6x8(int x, int y, const char *text);

/**
 * @brief Draw a single 12x16 font character.
 *
 * @param x Top-left X position.
 * @param y Top-left Y position.
 * @param c ASCII character to draw.
 */
void display_draw_char_12x16(int x, int y, char c);

/**
 * @brief Draw a null-terminated string in 12x16 font.
 *
 * @param x Starting X position.
 * @param y Starting Y position.
 * @param text Pointer to C string.
 */
void display_draw_text_12x16(int x, int y, const char *text);
/** @} */  // end text rendering

/**
 * @name Common UI status screens
 * @{
 */

/**
 * @brief Show a screen indicating Wi-Fi connection is in progress.
 */
void display_show_wifi_connecting(void);

/**
 * @brief Show a screen indicating Wi-Fi successfully connected.
 */
void display_show_wifi_connected(void);

/** @} */  // end UI screens

#ifdef __cplusplus
}
#endif

#endif  // DISPLAY_MANAGER_H
