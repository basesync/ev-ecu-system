/**
 * @file    stm32f1xx_hal_gpio.h
 * @brief   STM32 HAL GPIO peripheral stub
 *
 * @author  BaseSync Team
 * @version 1.0
 */

#ifndef STM32F1XX_HAL_GPIO_H
#define STM32F1XX_HAL_GPIO_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

/* ─── GPIO pin state ─────────────────────────────────────────────────────── */
typedef enum
{
    GPIO_PIN_RESET = 0U,   /**< Pin is LOW  (0V) */
    GPIO_PIN_SET   = 1U    /**< Pin is HIGH (3.3V) */
} GPIO_PinState;

/* ─── GPIO port type (opaque pointer in stub) ────────────────────────────── */
typedef struct { uint32_t dummy; } GPIO_TypeDef;

/* ─── GPIO port defines (match real STM32 HAL) ───────────────────────────── */
#define GPIOA  ((GPIO_TypeDef *)0x40010800U)
#define GPIOB  ((GPIO_TypeDef *)0x40010C00U)
#define GPIOC  ((GPIO_TypeDef *)0x40011000U)

/* ─── GPIO pin defines ───────────────────────────────────────────────────── */
#define GPIO_PIN_0   (0x0001U)
#define GPIO_PIN_1   (0x0002U)
#define GPIO_PIN_2   (0x0004U)
#define GPIO_PIN_5   (0x0020U)
#define GPIO_PIN_8   (0x0100U)
#define GPIO_PIN_13  (0x2000U)

/* ─── GPIO function declarations ─────────────────────────────────────────── */
/**
 * @brief Read the state of a GPIO pin.
 * @param GPIOx  GPIO port (GPIOA, GPIOB, etc.)
 * @param GPIO_Pin  Pin mask (GPIO_PIN_0, GPIO_PIN_1, etc.)
 * @retval GPIO_PIN_SET or GPIO_PIN_RESET
 */
GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/**
 * @brief Write a value to a GPIO pin.
 */
void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState);

/**
 * @brief Toggle a GPIO pin state.
 */
void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

#endif /* STM32F1XX_HAL_GPIO_H */
