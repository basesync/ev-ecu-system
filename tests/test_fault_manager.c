/**
 * @file    test_fault_manager.c
 * @brief   Unit tests for fault_manager.c
 *
 * @details Tests for all fault threshold checks in fault_check_all(),
 *          fault_is_set(), and fault_get_name().
 *
 *          No mocks required — fault_check_all() is a pure function that
 *          only reads the sensor_data_t passed in. setUp()/tearDown() are
 *          kept minimal.
 *
 *          Total tests: 20
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

#include "Unity/unity.h"
#include "ev_types.h"
#include "ev_config.h"
#include "fault_manager.h"

/* ─── Test fixture helpers ────────────────────────────────────────────────── */

/**
 * @brief  Return a sensor_data_t with all values in the SAFE (no-fault) range.
 *
 * @details Used as the baseline for all tests — each test modifies only
 *          the field under test. This ensures tests are isolated.
 */
static sensor_data_t make_safe_data(void)
{
    sensor_data_t d;

    d.batt_temp_c  = 25.0f;   /* Room temperature — well below 60°C */
    d.motor_temp_c = 30.0f;   /* Well below 80°C */
    d.current_a    = 10.0f;   /* Well below 50A */
    d.voltage_v    = 48.0f;   /* Nominal pack voltage — between 35V and 55V */
    d.speed_rpm    = 0U;
    d.throttle_pct = 0U;
    d.brake_active = false;
    d.fault_switch = false;

    return d;
}

/* ─── setUp / tearDown ────────────────────────────────────────────────────── */

void fm_setUp(void)
{
    /* No module state to reset — fault_manager is stateless */
}

void fm_tearDown(void)
{
    /* Nothing to tear down */
}

/* ─── Tests: NULL pointer guard ──────────────────────────────────────────── */

void test_fault_check_null_data_returns_fault_invalid_data(void)
{
    fault_code_t result = fault_check_all(NULL);

    TEST_ASSERT_EQUAL_HEX8(FAULT_INVALID_DATA, result);
}

/* ─── Tests: No-fault baseline ───────────────────────────────────────────── */

void test_fault_check_all_safe_data_returns_fault_none(void)
{
    sensor_data_t data = make_safe_data();

    fault_code_t result = fault_check_all(&data);

    TEST_ASSERT_EQUAL_HEX8(FAULT_NONE, result);
}

/* ─── Tests: Battery over-temperature ────────────────────────────────────── */

void test_fault_check_batt_temp_below_threshold_no_fault(void)
{
    sensor_data_t data    = make_safe_data();
    data.batt_temp_c      = EV_BATT_TEMP_CRITICAL_C - 0.1f;   /* Just under */

    fault_code_t result   = fault_check_all(&data);

    TEST_ASSERT_FALSE(fault_is_set(result, FAULT_OVER_TEMP_BATT));
}

void test_fault_check_batt_temp_at_threshold_no_fault(void)
{
    /*
     * Threshold is STRICTLY greater than (>), not >=.
     * Exactly at threshold must NOT trigger fault.
     */
    sensor_data_t data    = make_safe_data();
    data.batt_temp_c      = EV_BATT_TEMP_CRITICAL_C;   /* Exactly at limit */

    fault_code_t result   = fault_check_all(&data);

    TEST_ASSERT_FALSE(fault_is_set(result, FAULT_OVER_TEMP_BATT));
}

void test_fault_check_batt_temp_above_threshold_sets_bit(void)
{
    sensor_data_t data    = make_safe_data();
    data.batt_temp_c      = EV_BATT_TEMP_CRITICAL_C + 0.1f;   /* Just over */

    fault_code_t result   = fault_check_all(&data);

    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_OVER_TEMP_BATT));
}

/* ─── Tests: Motor over-temperature ─────────────────────────────────────── */

void test_fault_check_motor_temp_above_threshold_sets_bit(void)
{
    sensor_data_t data    = make_safe_data();
    data.motor_temp_c     = EV_MOTOR_TEMP_CRITICAL_C + 0.1f;

    fault_code_t result   = fault_check_all(&data);

    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_OVER_TEMP_MOTOR));
}

void test_fault_check_motor_temp_at_threshold_no_fault(void)
{
    sensor_data_t data    = make_safe_data();
    data.motor_temp_c     = EV_MOTOR_TEMP_CRITICAL_C;

    fault_code_t result   = fault_check_all(&data);

    TEST_ASSERT_FALSE(fault_is_set(result, FAULT_OVER_TEMP_MOTOR));
}

/* ─── Tests: Over-current ────────────────────────────────────────────────── */

void test_fault_check_over_current_above_threshold_sets_bit(void)
{
    sensor_data_t data  = make_safe_data();
    data.current_a      = EV_CURRENT_CRITICAL_A + 0.1f;

    fault_code_t result = fault_check_all(&data);

    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_OVER_CURRENT));
}

void test_fault_check_negative_current_no_fault(void)
{
    /* Negative current = charging. Not a fault. */
    sensor_data_t data  = make_safe_data();
    data.current_a      = -5.0f;

    fault_code_t result = fault_check_all(&data);

    TEST_ASSERT_FALSE(fault_is_set(result, FAULT_OVER_CURRENT));
}

/* ─── Tests: Under-voltage ───────────────────────────────────────────────── */

void test_fault_check_under_voltage_below_threshold_sets_bit(void)
{
    sensor_data_t data  = make_safe_data();
    data.voltage_v      = EV_VOLTAGE_MIN_CRITICAL_V - 0.1f;

    fault_code_t result = fault_check_all(&data);

    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_UNDER_VOLTAGE));
}

void test_fault_check_voltage_at_min_threshold_no_fault(void)
{
    sensor_data_t data  = make_safe_data();
    data.voltage_v      = EV_VOLTAGE_MIN_CRITICAL_V;   /* Exactly at limit */

    fault_code_t result = fault_check_all(&data);

    TEST_ASSERT_FALSE(fault_is_set(result, FAULT_UNDER_VOLTAGE));
}

/* ─── Tests: Over-voltage ────────────────────────────────────────────────── */

void test_fault_check_over_voltage_above_threshold_sets_bit(void)
{
    sensor_data_t data  = make_safe_data();
    data.voltage_v      = EV_VOLTAGE_MAX_CRITICAL_V + 0.1f;

    fault_code_t result = fault_check_all(&data);

    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_OVER_VOLTAGE));
}

void test_fault_check_voltage_at_max_threshold_no_fault(void)
{
    sensor_data_t data  = make_safe_data();
    data.voltage_v      = EV_VOLTAGE_MAX_CRITICAL_V;

    fault_code_t result = fault_check_all(&data);

    TEST_ASSERT_FALSE(fault_is_set(result, FAULT_OVER_VOLTAGE));
}

/* ─── Tests: Manual fault trigger ───────────────────────────────────────── */

void test_fault_check_fault_switch_pressed_sets_manual_trigger(void)
{
    sensor_data_t data   = make_safe_data();
    data.fault_switch    = true;

    fault_code_t result  = fault_check_all(&data);

    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_MANUAL_TRIGGER));
}

void test_fault_check_fault_switch_not_pressed_no_manual_trigger(void)
{
    sensor_data_t data   = make_safe_data();
    data.fault_switch    = false;

    fault_code_t result  = fault_check_all(&data);

    TEST_ASSERT_FALSE(fault_is_set(result, FAULT_MANUAL_TRIGGER));
}

/* ─── Tests: Multiple simultaneous faults ────────────────────────────────── */

void test_fault_check_multiple_faults_all_bits_set(void)
{
    sensor_data_t data   = make_safe_data();
    data.batt_temp_c     = EV_BATT_TEMP_CRITICAL_C  + 1.0f;
    data.motor_temp_c    = EV_MOTOR_TEMP_CRITICAL_C + 1.0f;
    data.current_a       = EV_CURRENT_CRITICAL_A    + 1.0f;
    data.fault_switch    = true;

    fault_code_t result  = fault_check_all(&data);

    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_OVER_TEMP_BATT));
    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_OVER_TEMP_MOTOR));
    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_OVER_CURRENT));
    TEST_ASSERT_TRUE(fault_is_set(result, FAULT_MANUAL_TRIGGER));
}

/* ─── Tests: fault_is_set() helper ──────────────────────────────────────── */

void test_fault_is_set_returns_false_for_clear_bit(void)
{
    fault_code_t faults = FAULT_OVER_CURRENT;   /* Only bit 2 set */

    TEST_ASSERT_FALSE(fault_is_set(faults, FAULT_OVER_TEMP_BATT));
}

void test_fault_is_set_returns_true_for_set_bit(void)
{
    fault_code_t faults = FAULT_OVER_CURRENT;

    TEST_ASSERT_TRUE(fault_is_set(faults, FAULT_OVER_CURRENT));
}

/* ─── Tests: fault_get_name() ────────────────────────────────────────────── */

void test_fault_get_name_returns_none_for_no_fault(void)
{
    const char *name = fault_get_name(FAULT_NONE);

    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("NONE", name);
}

void test_fault_get_name_returns_correct_name_for_over_temp_batt(void)
{
    const char *name = fault_get_name(FAULT_OVER_TEMP_BATT);

    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("OVER_TEMP_BATT", name);
}
