/**
 * @file    main.c
 * @brief   EV ECU - Firmware Entry Point (Sprint 1 Stub)
 *
 * @details This is the firmware entry point. In Sprint 1, this is a
 *          skeleton that verifies the build system works end-to-end.
 *          Actual ECU logic is added from Sprint 2 onwards.
 *
 *          Sprint 1 stub behaviour:
 *          - Configures system clock
 *          - Outputs "EV ECU Initialised" over UART
 *          - Blinks the status LED to confirm the MCU is running
 *          - Loops forever (bare-metal superloop placeholder)
 *
 * @note    Sprint 2 additions:
 *          - HAL peripheral initialisation (ADC, TIM, CAN, USART)
 *          - sensor_init(), motor_init(), fault_manager_init()
 *          - Main superloop with sensor reads and state machine
 *
 * @author  BaseSync Team
 * @version 0.1.0 (Sprint 1 — Build system verification)
 * @date    2025
 */

/* --- Includes -------------------------------------------------------------- */
/* NOTE: STM32 HAL includes are added in Sprint 2 when CubeMX HAL is set up. */
/* For Sprint 1, this stub compiles without STM32 HAL to verify the         */
/* cross-compiler toolchain and build system work correctly.                 */
#include <stdint.h>
#include <stdbool.h>

#include "ev_types.h"    /* Shared type definitions */
#include "ev_config.h"   /* Configuration constants */

/* --- Private Constants -------------------------------------------------- */

/** Simple delay loop count (not accurate - calibrate in Sprint 2 with SysTick) */
#define DELAY_LOOP_COUNT    (1000000U)

/* --- Private Function Declarations --------------------------------------
 * Functions used only within main.c are declared here (not in a header)
 */
static void system_init(void);
static void led_blink(uint32_t count);
static void delay_ms_approx(uint32_t ms);

/* --- Main Entry Point -------------------------------------------------- */

/**
 * @brief  Firmware entry point.
 * @retval Never returns (bare-metal infinite loop)
 */
int main(void)
{
    /* --- Step 1: Initialise system --- */
    system_init();

    /* --- Step 2: Blink LED to signal boot OK --- */
    /* 3 rapid blinks = firmware started successfully */
    led_blink(3U);

    /* ---─ Step 3: Main loop --- */
    /* Sprint 1: Simple blink loop to confirm MCU is running  */
    /* Sprint 2: Replace with full EV state machine superloop */
    while (1)
    {
        led_blink(1U);
        delay_ms_approx(1000U);  /* Approximately 1 second */

        /* TODO(sprint2): Add sensor_read_all(&sensor_data)       */
        /* TODO(sprint2): Add fault_check_all(&sensor_data)       */
        /* TODO(sprint2): Add ev_sm_run()                         */
        /* TODO(sprint2): Add can_send_status(&ev_status)         */
        /* TODO(sprint2): Add logger_log_sensors(&sensor_data)    */
        /* TODO(sprint2): Feed the watchdog timer                 */
    }

    /* Should never reach here */
    return 0;
}

/* --- Private Function Implementations -----------------------------------
 * These are simplified stubs. Replace with real HAL calls in Sprint 2.
 */

/**
 * @brief  Initialise the system.
 * @note   Sprint 1: Placeholder. Sprint 2: Calls HAL_Init(), SystemClock_Config(), etc.
 */
static void system_init(void)
{
    /*
     * Sprint 2 additions here:
     *   HAL_Init();
     *   SystemClock_Config();
     *   MX_GPIO_Init();
     *   MX_ADC1_Init();
     *   MX_TIM1_Init();   // PWM for motor
     *   MX_TIM3_Init();   // Encoder for speed
     *   MX_USART1_UART_Init();
     *   MX_CAN_Init();
     *   MX_IWDG_Init();   // Watchdog
     *
     *   sensor_init();
     *   motor_init();
     *   fault_manager_init();
     *   can_init();
     *   logger_init();
     *   ev_sm_init();
     */
}

/**
 * @brief  Blink the onboard LED a specified number of times.
 * @param  count  Number of blink cycles
 * @note   Sprint 1: Empty stub (no HAL). Sprint 2: Uses HAL_GPIO_TogglePin().
 */
static void led_blink(uint32_t count)
{
    uint32_t i;
    for (i = 0U; i < count; i++)
    {
        /*
         * Sprint 2: HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
         *           delay_ms_approx(200U);
         *           HAL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
         *           delay_ms_approx(200U);
         */
        (void)i; /* Suppress unused variable warning in Sprint 1 */
    }
}

/**
 * @brief  Approximate millisecond delay using a busy loop.
 * @param  ms  Approximate delay in milliseconds
 * @note   NOT accurate. Sprint 2: Replace with HAL_Delay(ms) using SysTick.
 */
static void delay_ms_approx(uint32_t ms)
{
    volatile uint32_t count = ms * (DELAY_LOOP_COUNT / 1000U);
    while (count > 0U)
    {
        count--;
    }
}
