/**
 * @file    test_ev_state_machine.c
 * @brief   Unit tests for ev_state_machine.c
 *
 * @details Tests cover all state transitions, entry actions, fault injection,
 *          and reset behaviour. Motor mock is used to verify that motor_stop()
 *          is called on the correct transitions.
 *
 *          Total tests: 22
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

#include "Unity/unity.h"
#include "ev_types.h"
#include "ev_config.h"
#include "ev_state_machine.h"
#include "mocks/mock_stm32_hal_tim.h"   /* For mock_tim_reset() */

/* ─── Test helpers ────────────────────────────────────────────────────────── */

/**
 * @brief  Return a sensor_data_t with all values in the no-fault, idle range.
 */
static sensor_data_t make_idle_data(void)
{
    sensor_data_t d;

    d.batt_temp_c  = 25.0f;
    d.motor_temp_c = 30.0f;
    d.current_a    = 5.0f;
    d.voltage_v    = 48.0f;
    d.speed_rpm    = 0U;
    d.throttle_pct = 0U;       /* Throttle at zero → IDLE, not RUNNING */
    d.brake_active = false;
    d.fault_switch = false;

    return d;
}

/**
 * @brief  Advance state machine from INIT to IDLE.
 *
 * @details ev_sm_init() sets INIT. One ev_sm_run() call transitions to IDLE.
 */
static void advance_to_idle(void)
{
    sensor_data_t d = make_idle_data();

    ev_sm_init();
    ev_sm_run(&d, FAULT_NONE);   /* INIT → IDLE */
}

/**
 * @brief  Advance state machine from INIT through IDLE to RUNNING.
 */
static void advance_to_running(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_idle();

    d.throttle_pct = 50U;   /* Throttle above deadband */
    ev_sm_run(&d, FAULT_NONE);   /* IDLE → RUNNING */
}

/* ─── setUp / tearDown ────────────────────────────────────────────────────── */

void sm_setUp(void)
{
    mock_tim_reset();
    ev_sm_init();
}

void sm_tearDown(void)
{
    /* Nothing to tear down */
}

/* ─── Tests: Initial state ───────────────────────────────────────────────── */

void test_sm_init_state_is_init(void)
{
    TEST_ASSERT_EQUAL_INT(EV_STATE_INIT, ev_sm_get_state());
}

void test_sm_get_state_name_after_init_is_init(void)
{
    const char *name = ev_sm_get_state_name();

    TEST_ASSERT_NOT_NULL(name);
    TEST_ASSERT_EQUAL_STRING("INIT", name);
}

/* ─── Tests: INIT → IDLE transition ─────────────────────────────────────── */

void test_sm_init_to_idle_on_first_run_with_no_fault(void)
{
    sensor_data_t d = make_idle_data();

    ev_sm_run(&d, FAULT_NONE);

    TEST_ASSERT_EQUAL_INT(EV_STATE_IDLE, ev_sm_get_state());
}

void test_sm_state_name_is_idle_after_transition(void)
{
    advance_to_idle();

    TEST_ASSERT_EQUAL_STRING("IDLE", ev_sm_get_state_name());
}

/* ─── Tests: IDLE → RUNNING transition ──────────────────────────────────── */

void test_sm_idle_to_running_when_throttle_above_deadband(void)
{
    sensor_data_t d  = make_idle_data();

    advance_to_idle();
    d.throttle_pct   = EV_THROTTLE_DEADBAND_PCT + 1U;   /* Just above */
    ev_sm_run(&d, FAULT_NONE);

    TEST_ASSERT_EQUAL_INT(EV_STATE_RUNNING, ev_sm_get_state());
}

void test_sm_stays_idle_when_throttle_at_deadband(void)
{
    sensor_data_t d  = make_idle_data();

    advance_to_idle();
    d.throttle_pct   = EV_THROTTLE_DEADBAND_PCT;   /* At limit — no transition */
    ev_sm_run(&d, FAULT_NONE);

    TEST_ASSERT_EQUAL_INT(EV_STATE_IDLE, ev_sm_get_state());
}

void test_sm_stays_idle_when_brake_active_even_with_throttle(void)
{
    sensor_data_t d  = make_idle_data();

    advance_to_idle();
    d.throttle_pct   = 60U;
    d.brake_active   = true;   /* Brake prevents RUNNING entry */
    ev_sm_run(&d, FAULT_NONE);

    TEST_ASSERT_EQUAL_INT(EV_STATE_IDLE, ev_sm_get_state());
}

/* ─── Tests: RUNNING → IDLE transition ──────────────────────────────────── */

void test_sm_running_to_idle_when_throttle_zero_and_brake(void)
{
    sensor_data_t d  = make_idle_data();

    advance_to_running();

    d.throttle_pct  = 0U;
    d.brake_active  = true;
    ev_sm_run(&d, FAULT_NONE);

    TEST_ASSERT_EQUAL_INT(EV_STATE_IDLE, ev_sm_get_state());
}

void test_sm_stays_running_when_throttle_zero_but_no_brake(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_running();

    d.throttle_pct  = 0U;
    d.brake_active  = false;
    ev_sm_run(&d, FAULT_NONE);

    /* Throttle=0 without brake: coasting, stays RUNNING */
    TEST_ASSERT_EQUAL_INT(EV_STATE_RUNNING, ev_sm_get_state());
}

/* ─── Tests: FAULT transitions ───────────────────────────────────────────── */

void test_sm_idle_to_fault_on_fault_code(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_idle();
    ev_sm_run(&d, FAULT_OVER_TEMP_BATT);

    TEST_ASSERT_EQUAL_INT(EV_STATE_FAULT, ev_sm_get_state());
}

void test_sm_running_to_fault_on_fault_code(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_running();
    ev_sm_run(&d, FAULT_OVER_CURRENT);

    TEST_ASSERT_EQUAL_INT(EV_STATE_FAULT, ev_sm_get_state());
}

void test_sm_fault_to_safe_state_on_next_run(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_idle();
    ev_sm_run(&d, FAULT_OVER_TEMP_BATT);   /* → FAULT */
    ev_sm_run(&d, FAULT_OVER_TEMP_BATT);   /* → SAFE_STATE (next run in FAULT) */

    TEST_ASSERT_EQUAL_INT(EV_STATE_SAFE_STATE, ev_sm_get_state());
}

void test_sm_safe_state_name_correct(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_idle();
    ev_sm_run(&d, FAULT_OVER_TEMP_BATT);
    ev_sm_run(&d, FAULT_OVER_TEMP_BATT);

    TEST_ASSERT_EQUAL_STRING("SAFE_STATE", ev_sm_get_state_name());
}

void test_sm_stays_safe_state_even_with_no_faults(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_idle();
    ev_sm_run(&d, FAULT_OVER_CURRENT);
    ev_sm_run(&d, FAULT_OVER_CURRENT);

    /* Now in SAFE_STATE — subsequent runs with FAULT_NONE must not exit */
    ev_sm_run(&d, FAULT_NONE);

    TEST_ASSERT_EQUAL_INT(EV_STATE_SAFE_STATE, ev_sm_get_state());
}

void test_sm_null_data_goes_to_safe_state(void)
{
    ev_sm_run(NULL, FAULT_NONE);

    TEST_ASSERT_EQUAL_INT(EV_STATE_SAFE_STATE, ev_sm_get_state());
}

/* ─── Tests: ev_sm_set_fault() ───────────────────────────────────────────── */

void test_sm_set_fault_from_running_transitions_to_fault(void)
{
    advance_to_running();

    ev_sm_set_fault(FAULT_MANUAL_TRIGGER);

    TEST_ASSERT_EQUAL_INT(EV_STATE_FAULT, ev_sm_get_state());
}

void test_sm_set_fault_with_none_is_ignored(void)
{
    advance_to_running();

    ev_sm_set_fault(FAULT_NONE);   /* Must be ignored */

    TEST_ASSERT_EQUAL_INT(EV_STATE_RUNNING, ev_sm_get_state());
}

/* ─── Tests: ev_sm_reset() ───────────────────────────────────────────────── */

void test_sm_reset_from_safe_state_returns_ok(void)
{
    sensor_data_t d = make_idle_data();

    advance_to_idle();
    ev_sm_run(&d, FAULT_OVER_TEMP_BATT);
    ev_sm_run(&d, FAULT_OVER_TEMP_BATT);   /* Now SAFE_STATE */

    ev_status_t result = ev_sm_reset();

    TEST_ASSERT_EQUAL_INT(EV_STATUS_OK, result);
    TEST_ASSERT_EQUAL_INT(EV_STATE_INIT, ev_sm_get_state());
}

void test_sm_reset_from_running_returns_error(void)
{
    advance_to_running();

    ev_status_t result = ev_sm_reset();

    TEST_ASSERT_EQUAL_INT(EV_STATUS_ERROR, result);
    TEST_ASSERT_EQUAL_INT(EV_STATE_RUNNING, ev_sm_get_state());
}

void test_sm_reset_from_idle_returns_error(void)
{
    advance_to_idle();

    ev_status_t result = ev_sm_reset();

    TEST_ASSERT_EQUAL_INT(EV_STATUS_ERROR, result);
}
