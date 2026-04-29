/**
 * @file    motor_ctrl.h
 * @brief   Motor Control module — public interface
 *
 * @details This module controls the traction motor speed via PWM output.
 *          It abstracts the STM32 TIM1 peripheral so higher-level modules
 *          never call HAL_TIM functions directly.
 *
 *          PWM Configuration:
 *            - Timer:     TIM1, Channel 1
 *            - Pin:       PA8
 *            - Frequency: 20kHz (EV_MOTOR_PWM_FREQUENCY_HZ)
 *            - Duty:      0% = stopped, 100% = full speed
 *
 *          Safety design:
 *            - motor_stop() is separate from motor_set_speed(0).
 *              motor_stop() is for safety-critical immediate stops (fault, brake).
 *              motor_set_speed(0) is for normal speed reduction.
 *
 *          Usage example:
 *          @code
 *            motor_init(&htim1);
 *            motor_set_speed(50);  // 50% throttle
 *            motor_stop();         // immediate stop on brake/fault
 *          @endcode
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

#ifndef MOTOR_CTRL_H
#define MOTOR_CTRL_H

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdint.h>
#include "ev_types.h"
#include "stm32f1xx_hal.h"

/* ─── Public Function Declarations ───────────────────────────────────────── */

/**
 * @brief  Initialise the motor control module.
 *
 * @details Stores the TIM1 handle and starts the PWM output at 0% duty cycle.
 *          Must be called once before any other motor_ctrl function.
 *
 * @param  htim  Pointer to the initialised TIM1 handle (PWM mode). Must not be NULL.
 *
 * @retval EV_STATUS_OK       Initialisation successful, PWM output started.
 * @retval EV_STATUS_INVALID  htim pointer is NULL.
 * @retval EV_STATUS_HAL_ERROR HAL_TIM_PWM_Start() failed.
 */
ev_status_t motor_init(TIM_HandleTypeDef *htim);

/**
 * @brief  Set the motor speed to a given duty cycle percentage.
 *
 * @details Calculates the PWM compare register value from the percentage
 *          and the timer's auto-reload value, then updates TIM1_CH1.
 *
 *          Does NOT apply a soft-start ramp — use motor_soft_start() for that.
 *          This function sets the speed immediately.
 *
 * @param  speed_pct  Target speed as a percentage (0 = stopped, 100 = full).
 *                    Values above 100 are rejected.
 *
 * @retval EV_STATUS_OK      Speed set successfully.
 * @retval EV_STATUS_INVALID speed_pct > EV_MOTOR_MAX_DUTY_PCT (100).
 * @retval EV_STATUS_NOT_READY motor_init() has not been called.
 */
ev_status_t motor_set_speed(uint8_t speed_pct);

/**
 * @brief  Immediately stop the motor (PWM duty cycle = 0%).
 *
 * @details This is the safety-critical stop function. It sets duty to 0
 *          immediately without any ramp. Used for:
 *            - Brake switch pressed
 *            - Fault condition detected
 *            - Emergency stop command
 *
 *          Intentionally always returns EV_STATUS_OK so callers can use
 *          it without checking the return value in interrupt context.
 *
 * @retval EV_STATUS_OK Always (stop is always safe to call).
 */
ev_status_t motor_stop(void);

/**
 * @brief  Ramp motor speed from current duty to target duty over ~500ms.
 *
 * @details Gradually increases PWM duty cycle to avoid:
 *            - Inrush current spikes that drain the battery suddenly
 *            - Mechanical shock to the drivetrain
 *
 *          The ramp steps in 1% increments with a delay between each step.
 *          If the motor is already faster than the target, it immediately
 *          sets the speed (no ramp-down — that would feel like coasting).
 *
 * @param  target_pct  Target speed percentage (0–100).
 *
 * @retval EV_STATUS_OK      Ramp completed, motor at target speed.
 * @retval EV_STATUS_INVALID target_pct > 100.
 * @retval EV_STATUS_NOT_READY motor_init() not called.
 */
ev_status_t motor_soft_start(uint8_t target_pct);

/**
 * @brief  Get the current motor PWM duty cycle.
 *
 * @details Returns the last value set by motor_set_speed() or motor_stop().
 *          Does NOT read from the hardware register — it tracks the value
 *          set by this module. This is intentional: it avoids a HAL read
 *          and is always consistent with what we commanded.
 *
 * @retval Current duty cycle percentage (0–100).
 */
uint8_t motor_get_speed(void);

#endif /* MOTOR_CTRL_H */
