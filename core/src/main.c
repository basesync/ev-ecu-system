/**
 * @file    main.c
 * @brief   Smart EV ECU — Firmware Entry Point (Sprint 2)
 *
 * @details Sprint 3 additions over Sprint 2:
 *          - Fault Manager integrated: fault_check_all() called every loop
 *          - EV State Machine integrated: ev_sm_run() drives state transitions
 *          - Motor control is now ONLY allowed in RUNNING state
 *          - CAN bus transmission: status and sensor frames every 100ms
 *          - Fault frame transmitted immediately on fault detection
 *          - Watchdog placeholder (HAL_IWDG_Refresh) — real IWDG in Sprint 5
 *          - ev_status_t struct populated and kept current
 *
 *          Sprint 4 will add:
 *          - UART Teleplot logger module (replaces printf stubs)
 *          - SIL simulation with Wokwi
 *
 * @note    HAL handles (hadc1, htim1, htim3) are declared as file-scope
 *          variables so that both main() and ISR callbacks can access them.
 *          In Sprint 5, CubeMX will generate these properly.
 *
 * @author  BaseSync Team
 * @version 0.3.0 (Sprint 3 — Fault Manager + State Machine + CAN Driver)
 * @date    2026
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>

#include "ev_types.h"
#include "ev_config.h"
#include "sensor_hal.h"
#include "motor_ctrl.h"
#include "fault_manager.h"
#include "ev_state_machine.h"
#include "can_driver.h"
#include "fault_logger.h"

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

/** Global EV status — updated every loop, transmitted every 100ms */
static ev_can_status_t g_ev_status;

/** System uptime counter in ms (updated in main loop) */
static uint32_t g_uptime_ms = 0U;

/** Loop counter used to rate-limit UART logging */
static uint32_t g_loop_counter = 0U;

/** Last fault code — used to detect new fault events for on-event CAN TX */
static fault_code_t g_last_fault_code = FAULT_NONE;

/* ─── Private Function Declarations ─────────────────────────────────────── */
static ev_status_t system_hal_init(void);
static void        main_loop_run(void);
static void         update_ev_status(fault_code_t faults);
static void         handle_motor_output(const sensor_data_t *data);
static void         can_periodic_transmit(void);
static void         watchdog_feed(void);
static void         delay_ms(uint32_t ms);

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

    /* fault_logger: stub in Sprint 3, real SPI init in Sprint 6 */
    (void)fault_logger_init(NULL);

    /* can_driver: stub in Sprint 3, real CAN_Start in Sprint 5 */
    (void)can_driver_init(NULL);

    /* State machine: initialise AFTER motor_ctrl — SM calls motor_stop on entry */
    ev_sm_init();

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
    fault_code_t current_faults;

    g_uptime_ms += MAIN_LOOP_PERIOD_MS;
    g_loop_counter++;

    /* ── 1. Read all sensors ── */
    /*
     * sensor_read_all() populates g_sensor_data with current values.
     * We ignore the return value here — Sprint 3 will check it and
     * pass bad reads to the fault manager.
     * TODO(sprint3): Check return value and feed to fault_manager.
     */
    (void)sensor_read_all(&g_sensor_data);

    /* ── 2. Check fault thresholds ── */
    current_faults = fault_check_all(&g_sensor_data);

    /* ── 3. Run state machine ── */
    /*
     * ev_sm_run() evaluates current_faults and sensor data to:
     *   - Transition state if conditions are met
     *   - Call motor_stop() automatically if transitioning to FAULT/SAFE_STATE
     */
    ev_sm_run(&g_sensor_data, current_faults);

    /* ── 4. Motor output (RUNNING state only) ── */
    handle_motor_output(&g_sensor_data);

    /* ── 5. Update status struct ── */
    update_ev_status(current_faults);

    /* ── 6. Send fault frame on new fault (on-event, not periodic) ── */
    if ((current_faults != FAULT_NONE) && (current_faults != g_last_fault_code))
    {
        (void)can_send_fault_frame(current_faults, g_uptime_ms);
        (void)fault_logger_write(current_faults, g_uptime_ms);
    }
    g_last_fault_code = current_faults;

    /* ── 7. Periodic 100ms CAN transmissions ── */
    if (g_loop_counter >= UART_LOG_EVERY_N_LOOPS)
    {
        can_periodic_transmit();
        g_loop_counter = 0U;
    }

    /* ── 8. Feed watchdog ── */
    watchdog_feed();
}

/**
 * @brief  Update motor PWM output based on current state and sensor data.
 *
 * @details Motor commands are ONLY issued when the state machine is in
 *          EV_STATE_RUNNING. In all other states (IDLE, FAULT, SAFE_STATE,
 *          INIT), the motor was already stopped by the state machine's
 *          entry action (priv_enter_state).
 *
 * @param  data  Current sensor data snapshot.
 */
static void handle_motor_output(const sensor_data_t *data)
{
    if (data == NULL)
    {
        return;
    }

    /* Only update motor speed when actively running */
    if (ev_sm_get_state() == EV_STATE_RUNNING)
    {
        if (data->brake_active == true)
        {
            /*
             * Brake pressed during RUNNING — stop immediately.
             * The state machine stays in RUNNING (brake alone doesn't change state
             * unless throttle is also zero). The motor is stopped here.
             */
            (void)motor_stop();
        }
        else
        {
            /* Normal: set speed from throttle */
            (void)motor_set_speed(data->throttle_pct);
        }
    }
    /*
     * In all other states, motor_stop() was already called by the state
     * machine transition. We do NOT call motor_stop() here again to avoid
     * redundant HAL calls in the tight 10ms loop.
     */
}

/**
 * @brief  Build the ev_status_t struct from current system state.
 *
 * @param  faults  Current fault bitmask.
 */
static void update_ev_status(fault_code_t faults)
{
    g_ev_status.state          = ev_sm_get_state();
    g_ev_status.active_faults  = faults;
    g_ev_status.motor_duty_pct = motor_get_speed();
    g_ev_status.soc_pct        = 0U;   /* TODO Sprint 5: Calculate from voltage */
    g_ev_status.uptime_ms      = g_uptime_ms;
}

/**
 * @brief  Transmit all three periodic CAN frames (every 100ms).
 */
static void can_periodic_transmit(void)
{
    (void)can_send_status(&g_ev_status);
    (void)can_send_sensor_pack1(&g_sensor_data);
    (void)can_send_sensor_pack2(&g_sensor_data);
}

/**
 * @brief  Feed the watchdog timer.
 *
 * @details Sprint 3: Placeholder — no real IWDG hardware in stub build.
 *          Sprint 5: Replace with HAL_IWDG_Refresh(&hiwdg).
 *          Called every main loop iteration (every 10ms).
 *          With 500ms timeout and 10ms period, there are 50 feeds per window.
 */
static void watchdog_feed(void)
{
    /*
     * TODO(sprint5): Replace with:
     *   HAL_IWDG_Refresh(&hiwdg);
     */
}

/**
 * @brief  Software delay (stub — replaced by HAL_Delay in Sprint 5).
 */
static void delay_ms(uint32_t ms)
{
    volatile uint32_t count = ms * 8000U;
    while (count > 0U)
    {
        count--;
    }
}
