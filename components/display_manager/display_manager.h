#ifndef DISPLAY_MANAGER_H
#define DISPLAY_MANAGER_H

#include "esp_err.h"
#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

esp_err_t display_init(void);
void display_clear(void);
void display_refresh(void);

// Drawing primitives
void display_draw_pixel(int x, int y, bool on);
void display_draw_icon(int x, int y, int w, int h, const uint8_t *bitmap);

// Text functions
void display_draw_char_6x8(int x, int y, char c);
void display_draw_text_6x8(int x, int y, const char *text);
void display_draw_char_12x16(int x, int y, char c);
void display_draw_text_12x16(int x, int y, const char *text);

#ifdef __cplusplus
}
#endif

#endif  // DISPLAY_MANAGER_H
