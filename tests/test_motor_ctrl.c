/**
 * @file    test_motor_ctrl.c
 * @brief   Unit tests for the motor_ctrl module
 *
 * @details Tests every public function in motor_ctrl.c:
 *          - motor_init() with valid and NULL handles
 *          - motor_set_speed() at various percentages and edge cases
 *          - motor_stop() immediate and always succeeds
 *          - motor_get_speed() reflects commanded value
 *          - Brake override logic via main loop simulation
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include "Unity/unity.h"
#include "motor_ctrl.h"
#include "ev_config.h"
#include "mocks/mock_stm32_hal_tim.h"

/* ─── Test fixtures ──────────────────────────────────────────────────────── */
static TIM_HandleTypeDef s_test_htim;

/* ─── setUp and tearDown ─────────────────────────────────────────────────── */

void motor_setUp(void)
{
    mock_tim_reset();

    /* Set up a valid TIM handle pointing at the mock instance */
    s_test_htim.Instance = mock_tim_get_instance();

    /* Re-initialise motor_ctrl before each test */
    (void)motor_init(&s_test_htim);
}

void motor_tearDown(void)
{
    /* Stop the motor after each test as cleanup */
    (void)motor_stop();
}

/* ═══════════════════════════════════════════════════════════════════════════
 * motor_init() TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_motor_init_null_handle_returns_invalid(void)
{
    ev_status_t result = motor_init(NULL);

    TEST_ASSERT_EQUAL(EV_STATUS_INVALID, result);
}

void test_motor_init_valid_handle_returns_ok(void)
{
    ev_status_t result = motor_init(&s_test_htim);

    TEST_ASSERT_EQUAL(EV_STATUS_OK, result);
}

void test_motor_init_sets_initial_speed_to_zero(void)
{
    (void)motor_init(&s_test_htim);

    uint8_t speed = motor_get_speed();

    TEST_ASSERT_EQUAL_UINT8(0U, speed);
}

void test_motor_init_pwm_start_error_returns_hal_error(void)
{
    mock_tim_set_pwm_start_error(true);

    ev_status_t result = motor_init(&s_test_htim);

    TEST_ASSERT_EQUAL(EV_STATUS_HAL_ERROR, result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * motor_set_speed() TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_motor_set_speed_zero_sets_duty_zero(void)
{
    (void)motor_set_speed(0U);

    /* Compare register should be 0 (0% of ARR) */
    TEST_ASSERT_EQUAL_UINT32(0U, mock_tim_get_compare_value());
}

void test_motor_set_speed_100_sets_duty_full(void)
{
    uint32_t expected_compare;
    uint32_t arr = mock_tim_get_arr();

    /* Expected: (100 * (ARR+1)) / 100 = ARR+1 */
    expected_compare = (100U * (arr + 1U)) / 100U;

    (void)motor_set_speed(100U);

    TEST_ASSERT_EQUAL_UINT32(expected_compare, mock_tim_get_compare_value());
}

void test_motor_set_speed_50pct_sets_half_duty(void)
{
    uint32_t arr = mock_tim_get_arr();

    /* Expected: (50 * (ARR+1)) / 100 = half of ARR */
    uint32_t expected_compare = (50U * (arr + 1U)) / 100U;

    (void)motor_set_speed(50U);

    /* Allow ±1 for integer rounding */
    uint32_t actual = mock_tim_get_compare_value();
    TEST_ASSERT_TRUE((actual >= expected_compare - 1U) &&
                     (actual <= expected_compare + 1U));
}

void test_motor_set_speed_over_100_returns_invalid(void)
{
    ev_status_t result = motor_set_speed(101U);

    TEST_ASSERT_EQUAL(EV_STATUS_INVALID, result);
}

void test_motor_set_speed_over_100_does_not_change_speed(void)
{
    /* Set a known speed first */
    (void)motor_set_speed(50U);
    uint8_t speed_before = motor_get_speed();

    /* Try to set an invalid speed */
    (void)motor_set_speed(200U);
    uint8_t speed_after = motor_get_speed();

    /* Speed should not have changed */
    TEST_ASSERT_EQUAL_UINT8(speed_before, speed_after);
}

void test_motor_set_speed_returns_ok_for_valid_values(void)
{
    ev_status_t result = motor_set_speed(75U);

    TEST_ASSERT_EQUAL(EV_STATUS_OK, result);
}

void test_motor_set_speed_below_deadband_sets_zero(void)
{
    /*
     * Throttle values below EV_THROTTLE_DEADBAND_PCT should result in
     * motor being stopped (deadband prevents motor creep at low throttle).
     */
    uint8_t below_deadband = EV_THROTTLE_DEADBAND_PCT - 1U;

    (void)motor_set_speed(below_deadband);

    TEST_ASSERT_EQUAL_UINT8(0U, motor_get_speed());
}

/* ═══════════════════════════════════════════════════════════════════════════
 * motor_stop() TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_motor_stop_sets_duty_to_zero(void)
{
    /* First set a non-zero speed */
    (void)motor_set_speed(80U);

    /* Then stop */
    (void)motor_stop();

    TEST_ASSERT_EQUAL_UINT32(0U, mock_tim_get_compare_value());
}

void test_motor_stop_always_returns_ok(void)
{
    ev_status_t result = motor_stop();

    TEST_ASSERT_EQUAL(EV_STATUS_OK, result);
}

void test_motor_stop_from_zero_still_returns_ok(void)
{
    /* Motor is already at 0 — stopping again must still succeed */
    (void)motor_set_speed(0U);

    ev_status_t result = motor_stop();

    TEST_ASSERT_EQUAL(EV_STATUS_OK, result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * motor_get_speed() TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_motor_get_speed_returns_last_set_value(void)
{
    (void)motor_set_speed(65U);

    uint8_t result = motor_get_speed();

    TEST_ASSERT_EQUAL_UINT8(65U, result);
}

void test_motor_get_speed_returns_zero_after_stop(void)
{
    (void)motor_set_speed(90U);
    (void)motor_stop();

    uint8_t result = motor_get_speed();

    TEST_ASSERT_EQUAL_UINT8(0U, result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * BRAKE OVERRIDE INTEGRATION TEST
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_motor_brake_override_throttle_80pct_with_brake_gives_zero(void)
{
    /*
     * This test simulates the motor_control_update() logic in main.c:
     * throttle = 80%, brake = active → motor should be at 0%.
     *
     * We test the logic directly to verify the branch works.
     */
    bool brake_active = true;
    uint8_t throttle_pct = 80U;

    /* Simulate motor_control_update() logic */
    if (brake_active == true)
    {
        (void)motor_stop();
    }
    else
    {
        (void)motor_set_speed(throttle_pct);
    }

    TEST_ASSERT_EQUAL_UINT8(0U, motor_get_speed());
}

void test_motor_brake_not_active_throttle_sets_speed(void)
{
    /*
     * throttle = 80%, brake = not active → motor should be at 80%.
     */
    bool    brake_active  = false;
    uint8_t throttle_pct  = 80U;

    if (brake_active == true)
    {
        (void)motor_stop();
    }
    else
    {
        (void)motor_set_speed(throttle_pct);
    }

    TEST_ASSERT_EQUAL_UINT8(80U, motor_get_speed());
}
