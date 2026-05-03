/**
 * @file    mock_stm32_hal_tim.h
 * @brief   Mock STM32 Timer HAL for unit testing
 * @author  BaseSync Team
 */

#ifndef MOCK_STM32_HAL_TIM_H
#define MOCK_STM32_HAL_TIM_H

#include "stm32f1xx_hal_tim.h"
#include <stdbool.h>

/** Reset all timer mock state. */
void mock_tim_reset(void);

/** Set the encoder counter value (simulates motor rotating). */
void mock_tim_set_encoder_count(uint32_t count);

/** Get the last compare value written (used to verify PWM duty). */
uint32_t mock_tim_get_compare_value(void);

/** Get the ARR value set on the mock timer (used in duty calculations). */
uint32_t mock_tim_get_arr(void);

/** Set the ARR (period) of the mock timer. Default is 3599. */
void mock_tim_set_arr(uint32_t arr);

/** Force HAL_TIM_PWM_Start to return HAL_ERROR. */
void mock_tim_set_pwm_start_error(bool force_error);

/** @brief Get the shared mock TIM instance pointer. */
TIM_TypeDef *mock_tim_get_instance(void);

#endif /* MOCK_STM32_HAL_TIM_H */
