/**
 * @file    motor_ctrl.c
 * @brief   Motor Control module implementation
 *
 * @details Controls motor speed via TIM1_CH1 PWM output on PA8.
 *
 *          How PWM motor control works:
 *          ─────────────────────────────
 *          The STM32 timer generates a square wave at 20kHz.
 *          The "duty cycle" is what fraction of each period is HIGH:
 *
 *          Duty 0%:   ─────────────────────  (LOW always, motor off)
 *          Duty 50%:  ████─────████─────████  (on half the time, half speed)
 *          Duty 100%: █████████████████████   (HIGH always, full speed)
 *
 *          The motor driver IC (e.g., L298N) converts this PWM to average
 *          current through the motor windings.
 *
 *          Setting the duty cycle:
 *          ───────────────────────
 *          The timer has an "auto-reload register" (ARR) that sets the period.
 *          The "compare register" (CCR1) sets when in the period to go LOW.
 *
 *          duty_pct = (CCR1 / ARR) * 100
 *          So: CCR1 = (duty_pct * ARR) / 100
 *
 *          We read ARR at runtime from the timer handle so this code
 *          works regardless of what CubeMX configured for the PWM period.
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include "motor_ctrl.h"
#include "ev_config.h"

/* ─── Private Variables ──────────────────────────────────────────────────── */

/* TIM1 handle stored at init, used by all motor control functions */
static TIM_HandleTypeDef *s_htim1 = NULL;

/* Track current duty cycle so motor_get_speed() does not need a HAL read */
static uint8_t s_current_duty_pct = 0U;

/* Flag to track whether motor_init() succeeded */
static bool s_motor_initialised = false;

/* ─── Private Function Declarations ─────────────────────────────────────── */
static void priv_set_pwm_duty(uint8_t duty_pct);

/* ─── Public Function Implementations ───────────────────────────────────── */

/**
 * @brief Initialise the motor control module.
 */
ev_status_t motor_init(TIM_HandleTypeDef *htim)
{
    /* Validate input — NULL handle means no PWM output */
    if (htim == NULL)
    {
        return EV_STATUS_INVALID;
    }

    s_htim1 = htim;

    /*
     * Start PWM output at 0% duty cycle.
     * The motor will not move yet, but the PWM signal is running.
     * This ensures a clean, known state before any speed commands.
     */
    if (HAL_TIM_PWM_Start(s_htim1, TIM_CHANNEL_1) != HAL_OK)
    {
        return EV_STATUS_HAL_ERROR;
    }

    /* Set initial duty to 0 — motor is stopped */
    priv_set_pwm_duty(0U);
    s_current_duty_pct  = 0U;
    s_motor_initialised = true;

    return EV_STATUS_OK;
}

/**
 * @brief Set motor speed to a given duty cycle percentage.
 */
ev_status_t motor_set_speed(uint8_t speed_pct)
{
    /* Guard: module must be initialised */
    if (s_motor_initialised == false)
    {
        return EV_STATUS_NOT_READY;
    }

    /*
     * Validate the requested speed.
     * Anything above 100% is not a valid PWM duty cycle.
     * We return INVALID so the caller knows the request was ignored.
     */
    if (speed_pct > EV_MOTOR_MAX_DUTY_PCT)
    {
        return EV_STATUS_INVALID;
    }

    /* Apply throttle deadband: below minimum, treat as zero */
    if (speed_pct < EV_THROTTLE_DEADBAND_PCT)
    {
        speed_pct = 0U;
    }

    /* Set the hardware PWM duty cycle */
    priv_set_pwm_duty(speed_pct);

    /* Update our tracked value */
    s_current_duty_pct = speed_pct;

    return EV_STATUS_OK;
}

/**
 * @brief Immediately stop the motor (safety stop).
 */
ev_status_t motor_stop(void)
{
    /*
     * Set duty to 0 directly without going through motor_set_speed().
     * This bypasses all checks — we always want stop to succeed
     * even if the module is in an unusual state.
     *
     * We call priv_set_pwm_duty() directly instead of motor_set_speed(0)
     * to make it clear this is a special case.
     */
    priv_set_pwm_duty(0U);
    s_current_duty_pct = 0U;

    /* Always return OK — safety stop must never report failure */
    return EV_STATUS_OK;
}

/**
 * @brief Ramp motor speed from current to target over ~500ms.
 */
ev_status_t motor_soft_start(uint8_t target_pct)
{
    uint8_t  current;
    uint8_t  step;
    uint32_t delay_per_step_ms;

    /* Guard: module must be initialised */
    if (s_motor_initialised == false)
    {
        return EV_STATUS_NOT_READY;
    }

    /* Validate target */
    if (target_pct > EV_MOTOR_MAX_DUTY_PCT)
    {
        return EV_STATUS_INVALID;
    }

    current = s_current_duty_pct;

    /* If already at or above target, set immediately (no ramp needed) */
    if (current >= target_pct)
    {
        priv_set_pwm_duty(target_pct);
        s_current_duty_pct = target_pct;
        return EV_STATUS_OK;
    }

    /*
     * Ramp up from current to target.
     *
     * We want to ramp over EV_MOTOR_SOFTSTART_MS (500ms total).
     * Number of steps = target_pct - current (each step = 1%)
     * Delay per step  = total_ms / num_steps
     *
     * Example: current=0, target=80 → 80 steps → 500/80 = 6ms per step
     */
    uint8_t num_steps = target_pct - current;

    /*
     * Protect against division by zero.
     * (num_steps == 0 was already handled by the >= check above)
     */
    delay_per_step_ms = EV_MOTOR_SOFTSTART_MS / (uint32_t)num_steps;

    /* Minimum 1ms delay to prevent busy-waiting if steps are many */
    if (delay_per_step_ms < 1U)
    {
        delay_per_step_ms = 1U;
    }

    /* Ramp loop: increment duty 1% at a time */
    for (step = current + 1U; step <= target_pct; step++)
    {
        priv_set_pwm_duty(step);
        s_current_duty_pct = step;
        HAL_Delay(delay_per_step_ms);
    }

    return EV_STATUS_OK;
}

/**
 * @brief Get the current motor PWM duty cycle.
 */
uint8_t motor_get_speed(void)
{
    /*
     * Return the last commanded duty cycle.
     * We track this in s_current_duty_pct rather than reading
     * from the hardware register, which keeps this function simple
     * and avoids a HAL call.
     */
    return s_current_duty_pct;
}

/* ─── Private Function Implementations ──────────────────────────────────── */

/**
 * @brief  Write a duty cycle percentage directly to the PWM compare register.
 *
 * @details This is the lowest-level function — it does the actual hardware write.
 *          All other motor control functions route through here.
 *
 *          Formula:
 *            compare_value = (duty_pct * ARR) / 100
 *          where ARR is the timer auto-reload value (the PWM period in counts).
 *
 *          Example: ARR=1000, duty_pct=75 → compare_value=750
 *          The timer counts 0→1000, goes HIGH at 0, goes LOW at 750 → 75% duty.
 *
 * @param  duty_pct  Duty cycle as a percentage (0–100). Caller must validate.
 */
static void priv_set_pwm_duty(uint8_t duty_pct)
{
    uint32_t arr_value;
    uint32_t compare_value;

    /* If timer is not initialised, nothing to do */
    if (s_htim1 == NULL)
    {
        return;
    }

    /* Read the auto-reload register value (the PWM period in timer counts) */
    arr_value = __HAL_TIM_GET_AUTORELOAD(s_htim1);

    /*
     * Calculate the compare register value.
     * Integer multiplication then division avoids float for this hot path.
     * +1 on ARR because the timer counts from 0 to ARR inclusive.
     */
    compare_value = ((uint32_t)duty_pct * (arr_value + 1U)) / 100U;

    /* Write the compare value to TIM1 Channel 1 */
    __HAL_TIM_SET_COMPARE(s_htim1, TIM_CHANNEL_1, compare_value);
}
