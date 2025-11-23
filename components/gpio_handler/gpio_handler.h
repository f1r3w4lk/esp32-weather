#ifndef GPIO_HANDLER_H
#define GPIO_HANDLER_H

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Initialize the GPIO handler module.
 *
 * This function configures the GPIO pins used by the system,
 * such as buttons or other digital inputs.
 */
void gpio_handler_init(void);

/**
 * @brief Check if the configuration button is being pressed.
 *
 * This function reads the state of the hardware configuration button.
 *
 * @return true if the button is currently pressed,
 *         false otherwise.
 */
bool gpio_handler_is_config_button_pressed(void);

#ifdef __cplusplus
}
#endif

#endif  // GPIO_HANDLER_H
