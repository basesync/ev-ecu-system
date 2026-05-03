/**
 * @file    fault_manager.c
 * @brief   Fault Manager — threshold checking and fault bitmask construction
 *
 * @details Sprint 3 implementation. This module evaluates every fault
 *          threshold from ev_config.h against the sensor data snapshot
 *          passed in and returns a bitmask of all active faults.
 *
 *          Design decisions recorded in ADR-004 (Docs/ADR-004-fault-manager.md):
 *          - No global state — pure function design for testability
 *          - All 6 checks always evaluated (no short-circuit on first fault)
 *          - Data NULL treated as FAULT_INVALID_DATA to prevent silent unsafe op
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include "fault_manager.h"
#include <stddef.h>

/* ─── Private Helper Macro ────────────────────────────────────────────────── */

/**
 * @brief  Set a fault bit in a bitmask if a condition is true.
 *
 * @details Using a macro avoids repeating the if/set pattern 6 times.
 *          It is not a function because we want it inlined with zero overhead.
 *          The parentheses around all arguments prevent operator precedence bugs.
 */
#define SET_FAULT_IF(condition, mask, bit)  \
    do {                                    \
        if ((condition))                    \
        {                                   \
            (mask) |= (bit);                \
        }                                   \
    } while (0)

/* ─── Public Function Implementations ────────────────────────────────────── */

/**
 * @brief  Check all sensor readings against fault thresholds.
 */
fault_code_t fault_check_all(const sensor_data_t *data)
{
    fault_code_t code = FAULT_NONE;

    /* NULL data is a critical error — we cannot make safe decisions */
    if (data == NULL)
    {
        return FAULT_INVALID_DATA;
    }

    /*
     * Check 1 — Battery over-temperature
     * Threshold: EV_BATT_TEMP_CRITICAL_C = 60.0°C
     * Source: ev_config.h Section 1
     */
    SET_FAULT_IF(data->batt_temp_c > EV_BATT_TEMP_CRITICAL_C,
                 code, FAULT_OVER_TEMP_BATT);

    /*
     * Check 2 — Motor over-temperature
     * Threshold: EV_MOTOR_TEMP_CRITICAL_C = 80.0°C
     * Motors tolerate higher temperatures than Li-ion battery cells.
     */
    SET_FAULT_IF(data->motor_temp_c > EV_MOTOR_TEMP_CRITICAL_C,
                 code, FAULT_OVER_TEMP_MOTOR);

    /*
     * Check 3 — Over-current
     * Threshold: EV_CURRENT_CRITICAL_A = 50.0A
     * Positive = discharge direction. Negative current (charging) is not a fault.
     */
    SET_FAULT_IF(data->current_a > EV_CURRENT_CRITICAL_A,
                 code, FAULT_OVER_CURRENT);

    /*
     * Check 4 — Under-voltage
     * Threshold: EV_VOLTAGE_MIN_CRITICAL_V = 35.0V
     * Below this level the battery may be permanently damaged.
     */
    SET_FAULT_IF(data->voltage_v < EV_VOLTAGE_MIN_CRITICAL_V,
                 code, FAULT_UNDER_VOLTAGE);

    /*
     * Check 5 — Over-voltage
     * Threshold: EV_VOLTAGE_MAX_CRITICAL_V = 55.0V
     * Indicates charging failure or regenerative braking overcurrent.
     */
    SET_FAULT_IF(data->voltage_v > EV_VOLTAGE_MAX_CRITICAL_V,
                 code, FAULT_OVER_VOLTAGE);

    /*
     * Check 6 — Manual fault trigger
     * The physical fault switch on PB1 allows engineers to manually
     * inject a fault during testing to verify the fault response chain.
     */
    SET_FAULT_IF(data->fault_switch == true,
                 code, FAULT_MANUAL_TRIGGER);

    return code;
}

/**
 * @brief  Check if a specific fault bit is set in a fault bitmask.
 */
bool fault_is_set(fault_code_t faults, fault_code_t fault_bit)
{
    return ((faults & fault_bit) != FAULT_NONE);
}

/**
 * @brief  Return a human-readable name for the active fault.
 */
const char *fault_get_name(fault_code_t faults)
{
    /*
     * Check in priority order — lowest bit first (most safety-critical).
     * Returns the name of the FIRST (highest-priority) active fault.
     * If multiple faults are active, the caller should log them individually.
     */
    if (faults == FAULT_NONE)          { return "NONE"; }
    if (fault_is_set(faults, FAULT_OVER_TEMP_BATT))   { return "OVER_TEMP_BATT"; }
    if (fault_is_set(faults, FAULT_OVER_TEMP_MOTOR))  { return "OVER_TEMP_MOTOR"; }
    if (fault_is_set(faults, FAULT_OVER_CURRENT))     { return "OVER_CURRENT"; }
    if (fault_is_set(faults, FAULT_UNDER_VOLTAGE))    { return "UNDER_VOLTAGE"; }
    if (fault_is_set(faults, FAULT_OVER_VOLTAGE))     { return "OVER_VOLTAGE"; }
    if (fault_is_set(faults, FAULT_WATCHDOG))         { return "WATCHDOG"; }
    if (fault_is_set(faults, FAULT_CAN_ERROR))        { return "CAN_ERROR"; }
    if (fault_is_set(faults, FAULT_MANUAL_TRIGGER))   { return "MANUAL_TRIGGER"; }
    return "UNKNOWN";
}
