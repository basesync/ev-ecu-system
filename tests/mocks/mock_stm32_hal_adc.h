/**
 * @file    mock_stm32_hal_adc.h
 * @brief   Mock (fake) STM32 ADC HAL for unit testing
 *
 * @details This mock replaces the real STM32 HAL ADC functions during
 *          unit tests. Instead of reading real hardware registers, it
 *          returns values that we set programmatically in test code.
 *
 *          How to use in a test:
 *          @code
 *            // Set what the ADC will "read" for battery temp channel
 *            mock_adc_set_channel_value(ADC_CHANNEL_0, 2048U);
 *
 *            // Now call the real function under test
 *            float temp = sensor_read_batt_temp();
 *
 *            // Check it calculated correctly
 *            TEST_ASSERT_FLOAT_WITHIN(1.0f, 50.0f, temp);
 *          @endcode
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

#ifndef MOCK_STM32_HAL_ADC_H
#define MOCK_STM32_HAL_ADC_H

#include <stdint.h>
#include <stdbool.h>
#include "stm32f1xx_hal_adc.h"

/* Maximum number of ADC channels the mock supports */
#define MOCK_ADC_MAX_CHANNELS  (18U)

/* ─── Mock control functions (called by test code) ───────────────────────── */

/**
 * @brief  Reset all mock ADC values to zero.
 *         Call this in your test setUp() function.
 */
void mock_adc_reset(void);

/**
 * @brief  Set the value that the mock ADC will return for a given channel.
 *
 * @param  channel    ADC channel number (ADC_CHANNEL_0 through ADC_CHANNEL_4)
 * @param  adc_value  12-bit ADC value to return (0–4095)
 */
void mock_adc_set_channel_value(uint32_t channel, uint32_t adc_value);

/**
 * @brief  Force HAL_ADC_Start() to return HAL_ERROR (test error handling).
 * @param  force_error  true = return HAL_ERROR, false = return HAL_OK
 */
void mock_adc_set_start_error(bool force_error);

/**
 * @brief  Force HAL_ADC_PollForConversion() to return HAL_TIMEOUT.
 * @param  force_timeout  true = return HAL_TIMEOUT, false = return HAL_OK
 */
void mock_adc_set_poll_timeout(bool force_timeout);

#endif /* MOCK_STM32_HAL_ADC_H */
