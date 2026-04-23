/**
 * @file    main.c
<<<<<<< HEAD
 * @brief   Smart EV ECU — Firmware Entry Point (Sprint 2)
 *
 * @details Sprint 2 additions over Sprint 1:
 *          - Sensor HAL initialisation and periodic reads
 *          - Motor control initialisation and throttle → PWM mapping
 *          - Brake override: brake switch immediately cuts motor PWM
 *          - UART Teleplot-format output of all sensor values
 *          - Basic main superloop structure (foundation for state machine)
 *
 *          Sprint 3 will add:
 *          - Fault detection logic
 *          - EV state machine (INIT → IDLE → RUNNING → FAULT)
 *          - CAN bus transmission
 *          - Watchdog timer feed
 *
 * @note    HAL handles (hadc1, htim1, htim3) are declared as file-scope
 *          variables so that both main() and ISR callbacks can access them.
 *          In Sprint 5, CubeMX will generate these properly.
 *
 * @author  BaseSync Team
 * @version 0.2.0 (Sprint 2 — Sensor & Motor modules)
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "ev_types.h"
#include "ev_config.h"
#include "sensor_hal.h"
#include "motor_ctrl.h"

/* ─── Private Constants ──────────────────────────────────────────────────── */

/** Main loop period in milliseconds */
#define MAIN_LOOP_PERIOD_MS    (10U)

/** How many loop cycles between UART log transmissions (every 100ms) */
#define UART_LOG_EVERY_N_LOOPS (EV_LOGGER_PERIOD_MS / MAIN_LOOP_PERIOD_MS)

/* ─── Private Variables ──────────────────────────────────────────────────── */

/**
 * STM32 HAL handles.
 * In Sprint 5, CubeMX generates these as extern globals from main.c.
 * For now we declare them here as stubs — replace with real inits in Sprint 5.
 */
static ADC_HandleTypeDef hadc1;   /**< ADC1 handle — all 5 sensor channels  */
static TIM_HandleTypeDef htim1;   /**< TIM1 handle — motor PWM output        */
static TIM_HandleTypeDef htim3;   /**< TIM3 handle — encoder input           */

/** Global sensor data — updated every MAIN_LOOP_PERIOD_MS */
static sensor_data_t g_sensor_data;

/** Loop counter used to rate-limit UART logging */
static uint32_t g_loop_counter = 0U;

/* ─── Private Function Declarations ─────────────────────────────────────── */
static ev_status_t system_hal_init(void);
static void        main_loop_run(void);
static void        motor_control_update(const sensor_data_t *data);
static void        uart_log_sensors(const sensor_data_t *data);
static void        delay_ms(uint32_t ms);

/* ─── Main Entry Point ───────────────────────────────────────────────────── */

/**
 * @brief  Firmware entry point.
 * @retval Never returns.
 */
int main(void)
{
    ev_status_t init_status;

    /* ── Step 1: Initialise all hardware peripherals ── */
    init_status = system_hal_init();

    if (init_status != EV_STATUS_OK)
    {
        /*
         * Hardware init failed — we cannot run safely.
         * In Sprint 3, this will set a FAULT_INIT_FAILED code and
         * enter SAFE_STATE. For Sprint 2, we blink an error pattern
         * and loop forever rather than running with broken hardware.
         *
         * TODO(sprint3): Replace with fault_manager_set(FAULT_INIT_FAILED)
         */
        while (1)
        {
            /* Blink LED rapidly to signal init failure */
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            delay_ms(100U);
        }
    }

    /* ── Step 2: Initialise application modules ── */

    /* Initialise sensor HAL — must be done before any sensor read */
    if (sensor_init(&hadc1, &htim3) != EV_STATUS_OK)
    {
        /*
         * Sensor init failed — ADC or encoder setup problem.
         * TODO(sprint3): fault_manager_set(FAULT_SENSOR_INIT_FAILED)
         */
        while (1)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            delay_ms(200U);
        }
    }

    /* Initialise motor control — must be done before any speed command */
    if (motor_init(&htim1) != EV_STATUS_OK)
    {
        /*
         * Motor init failed — PWM timer setup problem.
         * TODO(sprint3): fault_manager_set(FAULT_MOTOR_INIT_FAILED)
         */
        while (1)
        {
            HAL_GPIO_TogglePin(GPIOC, GPIO_PIN_13);
            delay_ms(300U);
        }
    }

    /* ── Step 3: Signal successful boot ── */
    /*
     * Three slow blinks = system initialised OK.
     * When connected to the serial terminal, also print a boot message.
     */
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_RESET); /* LED on */
    delay_ms(500U);
    HAL_GPIO_WritePin(GPIOC, GPIO_PIN_13, GPIO_PIN_SET);   /* LED off */
    delay_ms(200U);

    /*
     * NOTE: printf() will work in Sprint 5 when UART is properly configured.
     * For Sprint 2 stub, this is a placeholder.
     * TODO(sprint5): Configure USART1 retarget for printf/puts.
     */

    /* ── Step 4: Enter main superloop ── */
    while (1)
    {
        main_loop_run();
        delay_ms(MAIN_LOOP_PERIOD_MS);
    }

    /* Unreachable — bare-metal systems never return from main */
    return 0;
}

/* ─── Private Function Implementations ──────────────────────────────────── */

/**
 * @brief  Initialise all STM32 hardware peripherals.
 *
 * @details In Sprint 2, this is a stub that sets up the HAL handle
 *          structures with placeholder values so the code compiles and links.
 *
 *          In Sprint 5, this function will be replaced by the proper
 *          HAL_Init(), SystemClock_Config(), MX_ADC1_Init(), MX_TIM1_Init(),
 *          MX_TIM3_Init(), MX_USART1_UART_Init() sequence generated by CubeMX.
 *
 * @retval EV_STATUS_OK always in Sprint 2 (no real HAL to fail).
 */
static ev_status_t system_hal_init(void)
{
    /*
     * Sprint 2: Initialise handle structs to known values.
     * The mock HAL functions in tests/mocks/ use these to return
     * controlled values during unit tests.
     *
     * Sprint 5 replacement:
=======
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
>>>>>>> main
     *   HAL_Init();
     *   SystemClock_Config();
     *   MX_GPIO_Init();
     *   MX_ADC1_Init();
<<<<<<< HEAD
     *   MX_TIM1_Init();
     *   MX_TIM3_Init();
     *   MX_USART1_UART_Init();
     *   MX_IWDG_Init();
     */
    hadc1.Instance = (uint32_t)0x40012400U;  /* ADC1 base address stub */
    htim1.Instance = NULL;                    /* TIM1 stub             */
    htim3.Instance = NULL;                    /* TIM3 stub             */

    /* Set a non-zero ARR so PWM duty calculation works (not divide-by-zero) */
    /* Real value comes from CubeMX configuration in Sprint 5 */
    /* For 20kHz PWM at 72MHz clock: ARR = 72000000/20000 - 1 = 3599 */
    static TIM_TypeDef tim1_instance = { .ARR = 3599U };
    static TIM_TypeDef tim3_instance = { .CNT = 0U     };
    htim1.Instance = &tim1_instance;
    htim3.Instance = &tim3_instance;

    return EV_STATUS_OK;
}

/**
 * @brief  Run one iteration of the main control loop.
 *
 * @details Called every MAIN_LOOP_PERIOD_MS milliseconds.
 *          Reads sensors, updates motor control, logs data.
 *
 *          Sprint 3 will add: fault_check_all(), ev_sm_run(), can_send_status()
 */
static void main_loop_run(void)
{
    g_loop_counter++;

    /* ── 1. Read all sensors ── */
    /*
     * sensor_read_all() populates g_sensor_data with current values.
     * We ignore the return value here — Sprint 3 will check it and
     * pass bad reads to the fault manager.
     * TODO(sprint3): Check return value and feed to fault_manager.
     */
    (void)sensor_read_all(&g_sensor_data);

    /* ── 2. Update motor control ── */
    motor_control_update(&g_sensor_data);

    /* ── 3. Log sensor data over UART (rate-limited to every 100ms) ── */
    if (g_loop_counter >= UART_LOG_EVERY_N_LOOPS)
    {
        uart_log_sensors(&g_sensor_data);
        g_loop_counter = 0U;
    }

    /*
     * Sprint 3 additions here:
     *   fault_check_all(&g_sensor_data);
     *   ev_sm_run();
     *   can_send_status(&ev_status);
     *   IWDG_refresh();   // feed the watchdog
=======
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
>>>>>>> main
     */
}

/**
<<<<<<< HEAD
 * @brief  Update motor PWM based on sensor inputs.
 *
 * @details Implements the core motor control logic:
 *          1. Brake switch pressed → motor stop immediately
 *          2. Throttle > deadband  → set motor to throttle %
 *          3. Throttle <= deadband → motor stop
 *
 *          Sprint 3 will add: do NOT drive motor in FAULT or SAFE states.
 *
 * @param  data  Pointer to current sensor data snapshot.
 */
static void motor_control_update(const sensor_data_t *data)
{
    if (data == NULL)
    {
        /* Safety: if data is invalid, stop the motor */
        (void)motor_stop();
        return;
    }

    if (data->brake_active == true)
    {
        /*
         * Brake pressed: ALWAYS stop the motor, regardless of throttle.
         * This is a fundamental safety rule — brake overrides throttle.
         * Using motor_stop() (not motor_set_speed(0)) for immediate effect.
         */
        (void)motor_stop();
    }
    else
    {
        /*
         * Brake released: motor speed follows throttle position.
         * motor_set_speed() handles the deadband internally.
         * TODO(sprint3): Only run this in EV_STATE_RUNNING.
         */
        (void)motor_set_speed(data->throttle_pct);
=======
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
>>>>>>> main
    }
}

/**
<<<<<<< HEAD
 * @brief  Output sensor data in Teleplot format over UART.
 *
 * @details Teleplot format: ">label:value\n"
 *          The Teleplot VS Code extension reads this from the serial port
 *          and plots live graphs for each label.
 *
 *          In Sprint 2, printf() is a stub (UART not yet configured).
 *          In Sprint 5, with USART1 retargeted, these will plot live.
 *
 * @param  data  Pointer to sensor data to log.
 */
static void uart_log_sensors(const sensor_data_t *data)
{
    if (data == NULL)
    {
        return;
    }

    /*
     * Teleplot format for live plotting:
     * Each line: >variable_name:value
     *
     * The VS Code Teleplot extension reads UART at 115200 baud and
     * automatically creates a live plot for each named variable.
     *
     * TODO(sprint5): Replace with HAL_UART_Transmit() calls when
     *                USART1 is configured and printf() is retargeted.
     */

    /* Sprint 2: printf calls are stubs — will output in Sprint 5 */
    (void)printf(">batt_temp:%.1f\n",   (double)data->batt_temp_c);
    (void)printf(">motor_temp:%.1f\n",  (double)data->motor_temp_c);
    (void)printf(">current:%.2f\n",     (double)data->current_a);
    (void)printf(">voltage:%.1f\n",     (double)data->voltage_v);
    (void)printf(">speed:%u\n",         (unsigned int)data->speed_rpm);
    (void)printf(">throttle:%u\n",      (unsigned int)data->throttle_pct);
    (void)printf(">brake:%d\n",         (int)data->brake_active);
    (void)printf(">motor_pwm:%u\n",     (unsigned int)motor_get_speed());
}

/**
 * @brief  Software delay loop.
 *
 * @details Sprint 2: busy-loop approximation.
 *          Sprint 5: replaced by HAL_Delay() using SysTick.
 *
 * @param  ms  Approximate delay in milliseconds.
 */
static void delay_ms(uint32_t ms)
{
    /*
     * Sprint 5 TODO: Replace entire function with HAL_Delay(ms).
     * HAL_Delay() uses the SysTick timer and is accurate to 1ms.
     */
    volatile uint32_t count = ms * 8000U;  /* Approximate for 72MHz clock */

=======
 * @brief  Approximate millisecond delay using a busy loop.
 * @param  ms  Approximate delay in milliseconds
 * @note   NOT accurate. Sprint 2: Replace with HAL_Delay(ms) using SysTick.
 */
static void delay_ms_approx(uint32_t ms)
{
    volatile uint32_t count = ms * (DELAY_LOOP_COUNT / 1000U);
>>>>>>> main
    while (count > 0U)
    {
        count--;
    }
}
