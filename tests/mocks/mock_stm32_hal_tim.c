/**
 * @file    mock_stm32_hal_tim.c
 * @brief   Mock STM32 Timer HAL implementation
 * @author  BaseSync Team
 */

#include "mock_stm32_hal_tim.h"
#include <stdbool.h>
#include <stddef.h>

/* ─── Mock state ─────────────────────────────────────────────────────────── */
static uint32_t s_encoder_count     = 0U;
static uint32_t s_compare_value     = 0U;
static uint32_t s_arr_value         = 3599U;  /* Default for 20kHz at 72MHz */
static bool     s_pwm_start_error   = false;

/* Shared TIM_TypeDef instance used by tests */
static TIM_TypeDef s_mock_tim_instance = { .CCR1 = 0U, .ARR = 3599U, .CNT = 0U };

/* ─── Mock control functions ─────────────────────────────────────────────── */

void mock_tim_reset(void)
{
    s_encoder_count             = 0U;
    s_compare_value             = 0U;
    s_arr_value                 = 3599U;
    s_pwm_start_error           = false;
    s_mock_tim_instance.CCR1    = 0U;
    s_mock_tim_instance.ARR     = 3599U;
    s_mock_tim_instance.CNT     = 0U;
}

void mock_tim_set_encoder_count(uint32_t count)
{
    s_encoder_count           = count;
    s_mock_tim_instance.CNT   = count;
}

uint32_t mock_tim_get_compare_value(void)
{
    return s_mock_tim_instance.CCR1;
}

uint32_t mock_tim_get_arr(void)
{
    return s_mock_tim_instance.ARR;
}

void mock_tim_set_arr(uint32_t arr)
{
    s_arr_value               = arr;
    s_mock_tim_instance.ARR   = arr;
}

void mock_tim_set_pwm_start_error(bool force_error)
{
    s_pwm_start_error = force_error;
}

/**
 * @brief Get the shared mock TIM instance pointer.
 * Tests use this to set up a TIM_HandleTypeDef.
 */
TIM_TypeDef *mock_tim_get_instance(void)
{
    return &s_mock_tim_instance;
}

/* ─── Mock HAL function implementations ─────────────────────────────────── */

HAL_StatusTypeDef HAL_TIM_PWM_Init(TIM_HandleTypeDef *htim) {
    (void)htim; return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Base_Init(TIM_HandleTypeDef *htim) {
    (void)htim; return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    (void)Channel;

    if (s_pwm_start_error == true)
    {
        return HAL_ERROR;
    }

    /* Link the mock instance to the handle so macros work correctly */
    if (htim != NULL)
    {
        htim->Instance = &s_mock_tim_instance;
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    (void)htim;
    (void)Channel;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef *htim, uint32_t Channel)
{
    (void)Channel;

    if (htim != NULL)
    {
        htim->Instance = &s_mock_tim_instance;
    }

    return HAL_OK;
}
