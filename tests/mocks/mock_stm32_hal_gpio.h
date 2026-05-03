/**
 * @file    mock_stm32_hal_gpio.h
 * @brief   Mock STM32 GPIO HAL for unit testing
 * @author  BaseSync Team
 */

#ifndef MOCK_STM32_HAL_GPIO_H
#define MOCK_STM32_HAL_GPIO_H

#include "stm32f1xx_hal_gpio.h"
#include <stdbool.h>

/* ─── Mock control functions ─────────────────────────────────────────────── */

/** Reset all GPIO mock pin states to HIGH (released). */
void mock_gpio_reset(void);

/**
 * @brief  Set what HAL_GPIO_ReadPin() returns for a specific port+pin.
 * @param  GPIOx    GPIO port
 * @param  GPIO_Pin Pin mask
 * @param  state    GPIO_PIN_SET (HIGH) or GPIO_PIN_RESET (LOW)
 */
void mock_gpio_set_pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState state);

/**
 * @brief  Get the last value written by HAL_GPIO_WritePin() for verification.
 */
GPIO_PinState mock_gpio_get_written_pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

void mock_hal_set_tick(uint32_t tick_ms);

#endif /* MOCK_STM32_HAL_GPIO_H */
