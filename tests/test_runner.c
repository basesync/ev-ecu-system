/**
 * @file    test_runner.c
 * @brief   Unity test runner - entry point for all unit tests
 *
 * @details This file contains main() for the test executable.
 *          Add a call to run each module's tests here.
 *          The test runner exits with code 0 (success) if all tests pass,
 *          or code 1 (failure) if any test fails.
 *          GitHub Actions uses the exit code to determine pass/fail.
 *
 * @author  BaseSync Team
 * @date    2025
 */

/* --- Includes ---------------------------------------------------------------- */
#include "Unity/unity.h"

/* --- External test function declarations ------------------------------------
 * Declare every test function from every test_*.c file here.
 */

/* From test_placeholder.c */
extern void test_unity_framework_is_operational(void);
extern void test_placeholder_integer_assertion(void);
extern void test_placeholder_boolean_assertion(void);

/* Sprint 2 onwards — uncomment as modules are added: */
/* extern void test_sensor_read_batt_temp_adc_zero_returns_min(void);     */
/* extern void test_sensor_read_throttle_midscale_returns_50pct(void);    */
/* extern void test_motor_set_speed_zero_sets_pwm_zero(void);             */
/* extern void test_fault_over_temp_sets_batt_fault_bit(void);            */

/* --- Main - Test Runner Entry Point ------------------------------------- */
int main(void)
{
    /* UNITY_BEGIN() resets the test counter and prepares for test output */
    UNITY_BEGIN();

    /* --- Sprint 1: Framework verification tests --- */
    RUN_TEST(test_unity_framework_is_operational);
    RUN_TEST(test_placeholder_integer_assertion);
    RUN_TEST(test_placeholder_boolean_assertion);

    /* --- Sprint 2 onwards: Add module tests here --- */
    /* RUN_TEST(test_sensor_read_batt_temp_adc_zero_returns_min);  */
    /* RUN_TEST(test_sensor_read_throttle_midscale_returns_50pct); */
    /* RUN_TEST(test_motor_set_speed_zero_sets_pwm_zero);          */
    /* RUN_TEST(test_fault_over_temp_sets_batt_fault_bit);         */

    /* UNITY_END() prints the summary and returns 0 (pass) or 1 (fail) */
    return UNITY_END();
}
