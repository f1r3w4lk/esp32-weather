#ifndef GPIO_HANDLER_H
#define GPIO_HANDLER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize gpio handler
void gpio_handler_init(void);

/* Read Config button */
bool gpio_handler_is_config_button_pressed(void);

#ifdef __cplusplus
}
#endif

#endif  // GPIO_HANDLER_H