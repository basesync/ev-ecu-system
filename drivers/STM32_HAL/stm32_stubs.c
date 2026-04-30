#include "stm32f1xx_hal.h"
#include <stddef.h>

/* --- GPIO Stubs --- */
void HAL_GPIO_TogglePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    (void)GPIOx;
    (void)GPIO_Pin;
}

void HAL_GPIO_WritePin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState) {
    (void)GPIOx;
    (void)GPIO_Pin;
    (void)PinState;
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef* GPIOx, uint16_t GPIO_Pin) {
    (void)GPIOx;
    (void)GPIO_Pin;
    return GPIO_PIN_RESET;
}

/* --- System Stubs --- */
uint32_t HAL_GetTick(void) {
    return 0;
}

/* --- ADC Stubs --- */
HAL_StatusTypeDef HAL_ADC_ConfigChannel(ADC_HandleTypeDef* hadc, ADC_ChannelConfTypeDef* sConfig) {
    (void)hadc;
    (void)sConfig;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_Start(ADC_HandleTypeDef* hadc) {
    (void)hadc;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef* hadc, uint32_t Timeout) {
    (void)hadc;
    (void)Timeout;
    return HAL_OK;
}

uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef* hadc) {
    (void)hadc;
    return 0;
}

HAL_StatusTypeDef HAL_ADC_Stop(ADC_HandleTypeDef* hadc) {
    (void)hadc;
    return HAL_OK;
}

/* --- Timer / PWM Stubs --- */
HAL_StatusTypeDef HAL_TIM_Encoder_Start(TIM_HandleTypeDef* htim, uint32_t Channel) {
    (void)htim;
    (void)Channel;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Start(TIM_HandleTypeDef* htim, uint32_t Channel) {
    (void)htim;
    (void)Channel;
    return HAL_OK;
}

HAL_StatusTypeDef HAL_TIM_PWM_Stop(TIM_HandleTypeDef* htim, uint32_t Channel) {
    (void)htim;
    (void)Channel;
    return HAL_OK;
}
