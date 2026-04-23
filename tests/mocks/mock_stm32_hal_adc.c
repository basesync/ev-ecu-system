/**
 * @file    mock_stm32_hal_adc.c
 * @brief   Mock STM32 ADC HAL implementation for unit testing
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

#include "mock_stm32_hal_adc.h"
#include <stdbool.h>
#include <string.h>

/* ─── Mock state ─────────────────────────────────────────────────────────── */

/* Stores the fake ADC values for each channel */
static uint32_t s_mock_adc_values[MOCK_ADC_MAX_CHANNELS];

/* Stores which channel was most recently configured (to return right value) */
static uint32_t s_current_channel = 0U;

/* Error injection flags */
static bool s_force_start_error    = false;
static bool s_force_poll_timeout   = false;

/* ─── Mock control functions ─────────────────────────────────────────────── */

void mock_adc_reset(void)
{
    (void)memset(s_mock_adc_values, 0, sizeof(s_mock_adc_values));
    s_current_channel    = 0U;
    s_force_start_error  = false;
    s_force_poll_timeout = false;
}

void mock_adc_set_channel_value(uint32_t channel, uint32_t adc_value)
{
    if (channel < MOCK_ADC_MAX_CHANNELS)
    {
        s_mock_adc_values[channel] = adc_value;
    }
}

void mock_adc_set_start_error(bool force_error)
{
    s_force_start_error = force_error;
}

void mock_adc_set_poll_timeout(bool force_timeout)
{
    s_force_poll_timeout = force_timeout;
}

/* ─── Mock HAL function implementations ─────────────────────────────────── */
/* These replace the real STM32 HAL functions during unit tests.            */

HAL_StatusTypeDef HAL_ADC_Init(ADC_HandleTypeDef* hadc) {
    (void)hadc; return HAL_OK;
}

void HAL_ADC_MspInit(ADC_HandleTypeDef* hadc) {
    (void)hadc;
}

HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef      *hadc,
                                         ADC_ChannelConfTypeDef *sConfig)
{
    (void)hadc;  /* Unused in mock — suppress warning */

    /* Remember which channel was configured so GetValue() returns the right one */
    if (sConfig != NULL)
    {
        s_current_channel = sConfig->Channel;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef *hadc)
{
    (void)hadc;  /* Unused in mock */

    if (s_force_start_error == true)
    {
        return HAL_ERROR;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc,
                                             uint32_t           Timeout)
{
    (void)hadc;
    (void)Timeout;

    if (s_force_poll_timeout == true)
    {
        return HAL_TIMEOUT;
    }

    return HAL_OK;
}

uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc)
{
    (void)hadc;

    /* Return the value that was set for the most recently configured channel */
    if (s_current_channel < MOCK_ADC_MAX_CHANNELS)
    {
        return s_mock_adc_values[s_current_channel];
    }

    return 0U;
}

HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef *hadc)
{
    (void)hadc;
    return HAL_OK;
}
