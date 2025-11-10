#include "display_manager.h"
#include "display_assets.h"
#include "esp_log.h"
#include "driver/i2c_master.h"
#include "esp_lcd_panel_io.h"
#include "esp_lcd_panel_vendor.h"
#include "esp_lcd_panel_ops.h"
#include "esp_lcd_panel_ssd1306.h"
#include <string.h>
#include <stdio.h>

#define I2C_HOST 0
#define I2C_SDA_GPIO 21
#define I2C_SCL_GPIO 22
#define I2C_FREQ_HZ 100000

#define LCD_H_RES 128
#define LCD_V_RES 32
#define LCD_I2C_ADDR 0x3C

static const char *TAG = "display_manager";
static i2c_master_bus_handle_t i2c_bus_handle = NULL;
static esp_lcd_panel_handle_t panel_handle = NULL;
static uint8_t framebuffer[LCD_H_RES * LCD_V_RES / 8];

// ======================= BASICS ==========================
static inline void clear_framebuffer(void)
{
    memset(framebuffer, 0, sizeof(framebuffer));
}

static inline void set_pixel(int x, int y, bool on)
{
    if (x < 0 || x >= LCD_H_RES || y < 0 || y >= LCD_V_RES)
        return;
    int page = y / 8;
    int bit = y % 8;
    int byte_idx = page * LCD_H_RES + x;
    if (on)
        framebuffer[byte_idx] |= (1 << bit);
    else
        framebuffer[byte_idx] &= ~(1 << bit);
}

void display_draw_pixel(int x, int y, bool on)
{
    set_pixel(x, y, on);
}

// ======================= DRAW ICON ==========================
void display_draw_icon(int x, int y, int w, int h, const uint8_t *bitmap)
{
    int bytes_per_row = (w + 7) / 8;  // 3 bytes per row for 24px width

    for (int row = 0; row < h; row++) {
        for (int col = 0; col < w; col++) {
            int byte_index = row * bytes_per_row + (col / 8);
            uint8_t byte = bitmap[byte_index];
            bool pixel_on = byte & (0x80 >> (col % 8));  // MSB = leftmost pixel

            if (pixel_on) {
                set_pixel(x + col, y + row, true);
            }
        }
    }
}

// ======================= TEXT ==============================

// Draw single char (6x8)
void display_draw_char_6x8(int x, int y, char c)
{
    if (c < 32 || c > 126)
        return;
    const uint8_t *chr = font6x8[c - 32];
    for (int col = 0; col < 6; col++) {
        uint8_t bits = chr[col];
        for (int row = 0; row < 8; row++) {
            if (bits & (1 << row))
                set_pixel(x + col, y + row, true);
        }
    }
}

// Draw string (6x8)
void display_draw_text_6x8(int x, int y, const char *text)
{
    while (*text) {
        display_draw_char_6x8(x, y, *text++);
        x += 6;
    }
}

// Draw double size char (12x16)
void display_draw_char_12x16(int x, int y, char c)
{
    if (c < 32 || c > 126)
        return;
    const uint8_t *chr = font6x8[c - 32];
    for (int col = 0; col < 6; col++) {
        uint8_t bits = chr[col];
        for (int row = 0; row < 8; row++) {
            if (bits & (1 << row)) {
                // 2x2 pixel block
                for (int dx = 0; dx < 2; dx++)
                    for (int dy = 0; dy < 2; dy++)
                        set_pixel(x + col * 2 + dx, y + row * 2 + dy, true);
            }
        }
    }
}

// Draw string (12x16)
void display_draw_text_12x16(int x, int y, const char *text)
{
    while (*text) {
        display_draw_char_12x16(x, y, *text++);
        x += 12;
    }
}

// ======================= PUBLIC CONTROL ====================
esp_err_t display_init(void)
{
    ESP_LOGI(TAG, "Initializing SSD1306...");

    i2c_master_bus_config_t i2c_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_HOST,
        .scl_io_num = I2C_SCL_GPIO,
        .sda_io_num = I2C_SDA_GPIO,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true,
    };
    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_config, &i2c_bus_handle));

    esp_lcd_panel_io_i2c_config_t io_config = {
        .dev_addr = LCD_I2C_ADDR,
        .scl_speed_hz = I2C_FREQ_HZ,
        .control_phase_bytes = 1,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
        .dc_bit_offset = 6,
    };

    esp_lcd_panel_io_handle_t io_handle = NULL;
    ESP_ERROR_CHECK(esp_lcd_new_panel_io_i2c(i2c_bus_handle, &io_config, &io_handle));

    esp_lcd_panel_dev_config_t panel_config = {
        .bits_per_pixel = 1,
        .reset_gpio_num = -1,
    };
    esp_lcd_panel_ssd1306_config_t ssd1306_cfg = { .height = LCD_V_RES };
    panel_config.vendor_config = &ssd1306_cfg;

    ESP_ERROR_CHECK(esp_lcd_new_panel_ssd1306(io_handle, &panel_config, &panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_reset(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_init(panel_handle));
    ESP_ERROR_CHECK(esp_lcd_panel_mirror(panel_handle, true, true));
    ESP_ERROR_CHECK(esp_lcd_panel_disp_on_off(panel_handle, true));

    clear_framebuffer();
    display_refresh();
    ESP_LOGI(TAG, "Display ready");
    return ESP_OK;
}

void display_clear(void)
{
    clear_framebuffer();
    display_refresh();
}

void display_refresh(void)
{
    esp_lcd_panel_draw_bitmap(panel_handle, 0, 0, LCD_H_RES, LCD_V_RES, framebuffer);
}
