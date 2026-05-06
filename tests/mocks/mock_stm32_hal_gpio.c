/**
 * @file    mock_stm32_hal_gpio.c
 * @brief   Mock STM32 GPIO HAL implementation
 * @author  BaseSync Team
 */

#include "mock_stm32_hal_gpio.h"
#include <string.h>

/* Simple lookup: store pin states for GPIOA, GPIOB, GPIOC (3 ports, 16 pins each) */
#define MOCK_NUM_PORTS  (3U)
#define MOCK_NUM_PINS   (16U)

/* Map port pointers to indices */
static GPIO_TypeDef * const s_port_map[MOCK_NUM_PORTS] = { GPIOA, GPIOB, GPIOC };
static GPIO_PinState s_read_states[MOCK_NUM_PORTS][MOCK_NUM_PINS];
static GPIO_PinState s_write_states[MOCK_NUM_PORTS][MOCK_NUM_PINS];

/* ─── Private helper ─────────────────────────────────────────────────────── */
static int priv_port_index(GPIO_TypeDef *GPIOx)
{
    int i;
    for (i = 0; i < (int)MOCK_NUM_PORTS; i++)
    {
        if (s_port_map[i] == GPIOx) { return i; }
    }
    return -1;
}

static int priv_pin_index(uint16_t GPIO_Pin)
{
    int i;
    for (i = 0; i < (int)MOCK_NUM_PINS; i++)
    {
        if (GPIO_Pin == (uint16_t)(1U << (uint16_t)i)) { return i; }
    }
    return -1;
}

/* ─── Mock control functions ─────────────────────────────────────────────── */

void mock_gpio_reset(void)
{
    uint32_t p, n;
    for (p = 0U; p < MOCK_NUM_PORTS; p++)
    {
        for (n = 0U; n < MOCK_NUM_PINS; n++)
        {
            s_read_states[p][n]  = GPIO_PIN_SET;   /* Default: HIGH = released */
            s_write_states[p][n] = GPIO_PIN_RESET;
        }
    }
}

void mock_gpio_set_pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState state)
{
    int port_idx = priv_port_index(GPIOx);
    int pin_idx  = priv_pin_index(GPIO_Pin);

    if ((port_idx >= 0) && (pin_idx >= 0))
    {
        s_read_states[port_idx][pin_idx] = state;
    }
}

GPIO_PinState mock_gpio_get_written_pin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    int port_idx = priv_port_index(GPIOx);
    int pin_idx  = priv_pin_index(GPIO_Pin);

    if ((port_idx >= 0) && (pin_idx >= 0))
    {
        return s_write_states[port_idx][pin_idx];
    }
    return GPIO_PIN_RESET;
}

/* ─── Mock HAL function implementations ─────────────────────────────────── */

void HAL_GPIO_Init(GPIO_TypeDef  *GPIOx, void *GPIO_Init) {
    (void)GPIOx; (void)GPIO_Init;
}
void HAL_GPIO_DeInit(GPIO_TypeDef  *GPIOx, uint32_t GPIO_Pin) {
    (void)GPIOx; (void)GPIO_Pin;
}

GPIO_PinState HAL_GPIO_ReadPin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    int port_idx = priv_port_index(GPIOx);
    int pin_idx  = priv_pin_index(GPIO_Pin);

    if ((port_idx >= 0) && (pin_idx >= 0))
    {
        return s_read_states[port_idx][pin_idx];
    }
    return GPIO_PIN_SET;   /* Default HIGH if unknown */
}

void HAL_GPIO_WritePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin, GPIO_PinState PinState)
{
    int port_idx = priv_port_index(GPIOx);
    int pin_idx  = priv_pin_index(GPIO_Pin);

    if ((port_idx >= 0) && (pin_idx >= 0))
    {
        s_write_states[port_idx][pin_idx] = PinState;
    }
}

void HAL_GPIO_TogglePin(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin)
{
    int port_idx = priv_port_index(GPIOx);
    int pin_idx  = priv_pin_index(GPIO_Pin);

    if ((port_idx >= 0) && (pin_idx >= 0))
    {
        s_write_states[port_idx][pin_idx] =
            (s_write_states[port_idx][pin_idx] == GPIO_PIN_SET)
            ? GPIO_PIN_RESET : GPIO_PIN_SET;
    }
}

/* ─── HAL_GetTick and HAL_Delay mocks (used for debounce) ────────────────── */

static uint32_t s_mock_tick = 0U;

/** Call this in tests to advance simulated time. */
void mock_hal_set_tick(uint32_t tick_ms)
{
    s_mock_tick = tick_ms;
}

uint32_t HAL_GetTick(void)
{
    return s_mock_tick;
}

void HAL_Delay(uint32_t delay_ms) { (void)delay_ms; }
