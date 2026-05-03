/**
 * @file    fault_manager.h
 * @brief   Fault Manager — threshold checking and fault code management
 *
 * @details This module is responsible for comparing all sensor readings
 *          against configured thresholds (ev_config.h) and building the
 *          active fault bitmask. It is the ONLY module allowed to set
 *          fault codes — all other modules read them.
 *
 *          Fault bits are defined in ev_types.h:
 *            FAULT_OVER_TEMP_BATT  = 0x01
 *            FAULT_OVER_TEMP_MOTOR = 0x02
 *            FAULT_OVER_CURRENT    = 0x04
 *            FAULT_UNDER_VOLTAGE   = 0x08
 *            FAULT_OVER_VOLTAGE    = 0x10
 *            FAULT_WATCHDOG        = 0x20
 *            FAULT_CAN_ERROR       = 0x40
 *            FAULT_MANUAL_TRIGGER  = 0x80
 *
 *          Typical call sequence in main loop (Sprint 3):
 *          @code
 *            sensor_read_all(&g_sensor_data);
 *            fault_code_t faults = fault_check_all(&g_sensor_data);
 *            ev_sm_set_fault(faults);
 *          @endcode
 *
 *          Design rule: fault_check_all() NEVER modifies global state.
 *          It only reads the sensor data passed in and returns a bitmask.
 *          This makes it trivially testable — no setUp/tearDown needed.
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

#ifndef FAULT_MANAGER_H
#define FAULT_MANAGER_H

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdint.h>
#include <stddef.h>
#include "ev_types.h"
#include "ev_config.h"

/* ─── Public Function Declarations ───────────────────────────────────────── */

/**
 * @brief  Check all sensor readings against configured fault thresholds.
 *
 * @details Evaluates every threshold defined in ev_config.h and sets the
 *          corresponding bit in the returned fault_code_t bitmask.
 *
 *          Checks performed:
 *            1. batt_temp_c   > EV_BATT_TEMP_CRITICAL_C  → FAULT_OVER_TEMP_BATT
 *            2. motor_temp_c  > EV_MOTOR_TEMP_CRITICAL_C → FAULT_OVER_TEMP_MOTOR
 *            3. current_a     > EV_CURRENT_CRITICAL_A    → FAULT_OVER_CURRENT
 *            4. voltage_v     < EV_VOLTAGE_MIN_CRITICAL_V → FAULT_UNDER_VOLTAGE
 *            5. voltage_v     > EV_VOLTAGE_MAX_CRITICAL_V → FAULT_OVER_VOLTAGE
 *            6. fault_switch  == true                     → FAULT_MANUAL_TRIGGER
 *
 *          All 6 checks are always evaluated — no early return on first fault.
 *          This ensures the complete fault picture is captured in one call.
 *
 *          This function is PURE — it has no side effects, no global reads or
 *          writes. Same inputs always produce the same output. Unit-testable
 *          with zero mocking required.
 *
 * @param  data  Pointer to sensor data snapshot. Must not be NULL.
 *               All fields of sensor_data_t are evaluated.
 *
 * @retval Bitmask of all active faults. FAULT_NONE (0x00) = no faults.
 *         If data == NULL, returns FAULT_INVALID_DATA (0xFF) — treat
 *         as a critical error and enter SAFE_STATE immediately.
 */
fault_code_t fault_check_all(const sensor_data_t *data);

/**
 * @brief  Check if a specific fault bit is set in a fault code.
 *
 * @details Convenience wrapper for bitmask operations. Avoids magic bitwise
 *          operations scattered through calling code.
 *
 *          Example:
 *          @code
 *            fault_code_t faults = fault_check_all(&data);
 *            if (fault_is_set(faults, FAULT_OVER_TEMP_BATT)) {
 *                // Battery temperature fault handling
 *            }
 *          @endcode
 *
 * @param  faults    Fault bitmask returned by fault_check_all().
 * @param  fault_bit Specific fault bit to check (e.g., FAULT_OVER_CURRENT).
 *
 * @retval true  The specified fault bit is set.
 * @retval false The specified fault bit is clear, or faults == FAULT_NONE.
 */
bool fault_is_set(fault_code_t faults, fault_code_t fault_bit);

/**
 * @brief  Return a human-readable string for a fault code.
 *
 * @details Returns the name of the HIGHEST-PRIORITY active fault in
 *          the bitmask. Priority order matches bit significance (LSB first).
 *          Used for UART diagnostic logging in main loop.
 *
 *          Example output: "OVER_TEMP_BATT", "OVER_CURRENT", "NONE"
 *
 * @param  faults  Fault bitmask to decode.
 *
 * @retval Pointer to a constant string literal. Never NULL.
 *         Do NOT free() the returned pointer.
 */
const char *fault_get_name(fault_code_t faults);

#endif /* FAULT_MANAGER_H */
