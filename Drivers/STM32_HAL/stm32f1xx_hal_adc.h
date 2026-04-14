/**
 * @file    stm32f1xx_hal_adc.h
 * @brief   STM32 HAL ADC peripheral stub — types and function declarations
 *
 * @details Stub header providing the minimum ADC types and function
 *          prototypes needed to compile sensor_hal.c.
 *          In Sprint 5+, replaced by the real STM32 HAL ADC header.
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

#ifndef STM32F1XX_HAL_ADC_H
#define STM32F1XX_HAL_ADC_H

#include <stdint.h>
#include "stm32f1xx_hal_gpio.h"   /* For HAL_StatusTypeDef if included standalone */

/* ─── ADC handle type ─────────────────────────────────────────────────────── */
/**
 * @brief Minimal ADC handle type.
 * The real type has many more fields. We only need the pointer to exist.
 */
typedef struct
{
    uint32_t Instance;   /**< ADC peripheral base address (e.g., ADC1) */
    uint32_t Channel;    /**< Currently selected ADC channel            */
    uint32_t State;      /**< Current ADC state flags                   */
} ADC_HandleTypeDef;

/* ─── ADC channel configuration type ────────────────────────────────────── */
typedef struct
{
    uint32_t Channel;      /**< ADC channel number (0–17)   */
    uint32_t Rank;         /**< Conversion rank (1 in single mode) */
    uint32_t SamplingTime; /**< Sampling time cycles         */
} ADC_ChannelConfTypeDef;

/* ─── ADC channel defines ────────────────────────────────────────────────── */
#define ADC_CHANNEL_0    (0U)
#define ADC_CHANNEL_1    (1U)
#define ADC_CHANNEL_2    (2U)
#define ADC_CHANNEL_3    (3U)
#define ADC_CHANNEL_4    (4U)
#define ADC_CHANNEL_5    (5U)

#define ADC_RANK_1                  (1U)
#define ADC_SAMPLETIME_239CYCLES_5  (7U)

/* ─── Timeout constants ──────────────────────────────────────────────────── */
#define HAL_MAX_DELAY    (0xFFFFFFFFU)

/* ─── ADC function declarations ──────────────────────────────────────────── */
/* These are implemented in:
 *   - Real HAL: stm32f1xx_hal_adc.c (added in Sprint 5)
 *   - Unit tests: Tests/mocks/mock_stm32_hal_adc.c
 */

/**
 * @brief Configure one ADC channel for the next conversion.
 */
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef       *hadc,
                                         ADC_ChannelConfTypeDef  *sConfig);

/**
 * @brief Start a single ADC conversion (polling mode).
 */
HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc);

/**
 * @brief Wait for an ADC conversion to complete.
 */
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc,
                                             uint32_t           Timeout);

/**
 * @brief Read the converted ADC value (12-bit: 0–4095).
 */
uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc);

/**
 * @brief Stop the ADC.
 */
HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc);

#endif /* STM32F1XX_HAL_ADC_H */
