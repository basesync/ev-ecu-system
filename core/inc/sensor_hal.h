/**
 * @file    sensor_hal.h
 * @brief   Sensor Hardware Abstraction Layer — public interface
 *
 * @details This module provides a clean interface for reading all physical
 *          sensors connected to the EV ECU. It abstracts the STM32 ADC,
 *          GPIO, and Timer peripherals so that higher-level modules
 *          (fault manager, state machine, logger) never call HAL directly.
 *
 *          Sensors managed by this module:
 *            - Battery temperature  (ADC channel 0, PA0, LM35)
 *            - Motor temperature    (ADC channel 1, PA1, LM35)
 *            - Battery current      (ADC channel 2, PA2, ACS712-20A)
 *            - Battery voltage      (ADC channel 3, PA3, resistor divider)
 *            - Throttle position    (ADC channel 4, PA4, potentiometer)
 *            - Motor speed          (TIM3 encoder mode, PA6/PA7)
 *            - Brake switch         (GPIO PB0, active-low)
 *            - Fault trigger switch (GPIO PB1, active-low)
 *
 *          Usage example:
 *          @code
 *            sensor_data_t data;
 *            if (sensor_init(&hadc1, &htim3) == EV_STATUS_OK) {
 *                sensor_read_all(&data);
 *                // data.batt_temp_c, data.throttle_pct etc. are now valid
 *            }
 *          @endcode
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

#ifndef SENSOR_HAL_H
#define SENSOR_HAL_H

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdint.h>
#include <stdbool.h>
#include "ev_types.h"
#include "ev_config.h"
#include "stm32f1xx_hal.h"

/* ─── Public Function Declarations ───────────────────────────────────────── */

/**
 * @brief  Initialise the sensor HAL module.
 *
 * @details Must be called once before any sensor read function.
 *          Stores the ADC and Timer handles for use in all subsequent reads.
 *          Does NOT start any conversion — reads are on-demand (polling).
 *
 * @param  hadc   Pointer to the initialised ADC handle (ADC1).
 *                Must not be NULL.
 * @param  htim   Pointer to the initialised Timer handle (TIM3, encoder mode).
 *                Must not be NULL.
 *
 * @retval EV_STATUS_OK       Initialisation successful.
 * @retval EV_STATUS_INVALID  One or both handle pointers are NULL.
 */
ev_status_t sensor_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim);

/**
 * @brief  Read all sensors and populate a sensor_data_t structure.
 *
 * @details This is the primary function called by the main loop each cycle.
 *          It reads every sensor in sequence and fills all fields of the
 *          provided sensor_data_t struct.
 *
 *          If any individual sensor read fails, the corresponding field is
 *          set to a safe default (0 or false) and the function returns
 *          EV_STATUS_ERROR so the caller can act on the partial data.
 *
 * @param  data  Pointer to sensor_data_t struct to populate. Must not be NULL.
 *
 * @retval EV_STATUS_OK       All sensors read successfully.
 * @retval EV_STATUS_INVALID  data pointer is NULL.
 * @retval EV_STATUS_ERROR    One or more sensor reads returned an error.
 * @retval EV_STATUS_NOT_READY sensor_init() has not been called yet.
 */
ev_status_t sensor_read_all(sensor_data_t *data);

/**
 * @brief  Read battery temperature from ADC channel 0 (PA0).
 *
 * @details Performs a single ADC conversion and converts the 12-bit raw value
 *          to degrees Celsius using the LM35 transfer function:
 *          temperature_C = (adc_voltage_V / EV_TEMP_SENSOR_MV_PER_DEG) * 1000
 *
 * @retval Battery temperature in degrees Celsius.
 *         Returns 0.0f if the ADC read fails (safe default: no false fault).
 */
float sensor_read_batt_temp(void);

/**
 * @brief  Read motor temperature from ADC channel 1 (PA1).
 *
 * @details Same LM35 conversion as battery temperature, different channel.
 *
 * @retval Motor temperature in degrees Celsius. Returns 0.0f on error.
 */
float sensor_read_motor_temp(void);

/**
 * @brief  Read battery current from ADC channel 2 (PA2).
 *
 * @details Uses the ACS712-20A Hall-effect current sensor transfer function:
 *          - Sensor output: 2.5V at 0A
 *          - Sensitivity: 100mV per Ampere
 *          - Positive value: battery discharging (powering the motor)
 *          - Negative value: battery charging (regenerative braking)
 *
 * @retval Battery current in Amperes (signed). Returns 0.0f on error.
 */
float sensor_read_current(void);

/**
 * @brief  Read battery pack voltage from ADC channel 3 (PA3).
 *
 * @details A resistor voltage divider scales the 48V battery pack voltage
 *          down to fit in the 0–3.3V ADC input range.
 *          Divider: R1=100kΩ, R2=10kΩ → ratio = (R1+R2)/R2 = 11
 *          battery_V = adc_voltage_V * 11.0
 *
 * @retval Battery pack voltage in Volts. Returns 0.0f on error.
 */
float sensor_read_voltage(void);

/**
 * @brief  Read motor speed from TIM3 encoder input (PA6/PA7).
 *
 * @details TIM3 is configured in encoder mode. The counter increments/
 *          decrements with each encoder pulse. This function reads the
 *          counter and converts it to RPM using the PPR constant.
 *
 * @retval Motor speed in RPM. Returns 0 if timer handle not initialised.
 */
uint16_t sensor_read_speed(void);

/**
 * @brief  Read throttle position from ADC channel 4 (PA4).
 *
 * @details A 10kΩ potentiometer provides 0–3.3V. This is linearly mapped
 *          to a 0–100% throttle percentage.
 *          Values are clamped to 100 maximum to prevent invalid motor commands.
 *
 * @retval Throttle position as a percentage (0 = released, 100 = full).
 *         Returns 0 on error.
 */
uint8_t sensor_read_throttle(void);

/**
 * @brief  Read brake switch state from GPIO PB0.
 *
 * @details The brake switch is wired active-low with an internal pull-up:
 *          - Switch open (not pressed):  PB0 = HIGH → returns false
 *          - Switch closed (pressed):    PB0 = LOW  → returns true
 *
 *          A 20ms software debounce is applied to prevent false triggers
 *          from switch bounce on rapid presses.
 *
 * @retval true  = brake is pressed.
 * @retval false = brake is released.
 */
bool sensor_read_brake(void);

/**
 * @brief  Read manual fault trigger switch from GPIO PB1.
 *
 * @details Same active-low logic as the brake switch.
 *          Used during testing to manually inject a fault and verify
 *          the fault manager and state machine respond correctly.
 *
 * @retval true  = fault switch is pressed (inject fault).
 * @retval false = fault switch is released.
 */
bool sensor_read_fault_switch(void);

/* ─── Sprint 5 — I2C Temperature Sensor Functions ────────────────────────── */

/**
 * @brief  Enable I2C-based battery temperature sensing (TMP102).
 *
 * @details STUB in Sprint 2–4. Implemented in Sprint 5.
 *          Calling with NULL keeps ADC simulation active.
 *          Calling with a valid hi2c1 handle switches sensor_read_batt_temp()
 *          to read from the TMP102 over I2C instead of the ADC potentiometer.
 *
 * @param  hi2c  Pointer to I2C1 handle. NULL = keep ADC simulation.
 *
 * @retval EV_STATUS_OK        I2C handle stored (or NULL accepted as stub).
 * @retval EV_STATUS_INVALID   hi2c non-NULL but TMP102 does not respond.
 * @retval EV_STATUS_NOT_READY sensor_init() has not been called first.
 */
ev_status_t sensor_enable_i2c_temp(I2C_HandleTypeDef *hi2c);

/**
 * @brief  Return which backend is in use for battery temperature reading.
 *
 * @retval EV_PROTO_STATUS_STUB   Using ADC simulation (Sprint 1–4 default).
 * @retval EV_PROTO_STATUS_ACTIVE Using real TMP102 over I2C (Sprint 5+).
 */
ev_proto_status_t sensor_get_temp_backend(void);

#endif /* SENSOR_HAL_H */
