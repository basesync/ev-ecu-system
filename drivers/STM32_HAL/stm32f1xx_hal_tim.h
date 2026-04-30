/**
 * @file    stm32f1xx_hal_tim.h
 * @brief   STM32 HAL Timer peripheral stub
 *
 * @author  BaseSync Team
 * @version 1.0
 */

#ifndef STM32F1XX_HAL_TIM_H
#define STM32F1XX_HAL_TIM_H

#include <stdint.h>
#include "stm32f1xx_hal.h"

/* ─── Timer channel defines ──────────────────────────────────────────────── */
#define TIM_CHANNEL_1   (0x00000000U)
#define TIM_CHANNEL_2   (0x00000004U)

/* ─── Timer instance type (opaque in stub) ───────────────────────────────── */
typedef struct
{
    uint32_t CR1;    /**< Control register 1  */
    uint32_t CR2;    /**< Control register 2  */
    uint32_t CNT;    /**< Counter value        */
    uint32_t ARR;    /**< Auto-reload register */
    uint32_t CCR1;   /**< Capture/Compare register 1 (PWM duty) */
} TIM_TypeDef;

/* ─── Timer handle type ──────────────────────────────────────────────────── */
typedef struct
{
    TIM_TypeDef *Instance;    /**< Timer peripheral base address */
    uint32_t     Channel;     /**< Active channel                */
    uint32_t     State;       /**< Timer state                   */
    uint32_t     Init_Period; /**< Auto-reload value (PWM period) */
} TIM_HandleTypeDef;

/* ─── Macro: set PWM compare value (controls duty cycle) ─────────────────── */
/**
 * @brief Set the PWM duty cycle via the capture/compare register.
 * Usage: __HAL_TIM_SET_COMPARE(&htim1, TIM_CHANNEL_1, compare_value);
 * where compare_value is between 0 and ARR (period).
 */
#define __HAL_TIM_SET_COMPARE(__HANDLE__, __CHANNEL__, __COMPARE__) \
    ((__HANDLE__)->Instance->CCR1 = (__COMPARE__))

/**
 * @brief Get the current auto-reload register value (PWM period).
 */
#define __HAL_TIM_GET_AUTORELOAD(__HANDLE__) \
    ((__HANDLE__)->Instance->ARR)

/**
 * @brief Get the current timer counter value (used for encoder mode).
 */
#define __HAL_TIM_GET_COUNTER(__HANDLE__) \
    ((__HANDLE__)->Instance->CNT)

/* ─── Timer function declarations ────────────────────────────────────────── */

/**
 * @brief Start PWM generation on a timer channel.
 */
HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel);

/**
 * @brief Stop PWM generation on a timer channel.
 */
HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel);

/**
 * @brief Start encoder interface mode.
 */
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel);

#endif /* STM32F1XX_HAL_TIM_H */
