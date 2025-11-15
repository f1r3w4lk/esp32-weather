#include "gpio_handler.h"
#include "driver/gpio.h"
#include "esp_log.h"

static const char *TAG = "GPIO_HANDLER";

/* Define the cfg pin */
#define CONFIG_BUTTON_GPIO  5   // GPIO 5

void gpio_handler_init(void)
{
    ESP_LOGI(TAG, "Initializing GPIO handler...");

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << CONFIG_BUTTON_GPIO),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };

    gpio_config(&io_conf);
}

/* Pressed button == low level */
bool gpio_handler_is_config_button_pressed(void)
{
    return (gpio_get_level(CONFIG_BUTTON_GPIO) == 0);
}
