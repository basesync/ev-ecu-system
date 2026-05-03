/**
 * @file    test_placeholder.c
 * @brief   Sprint 1 placeholder unit test
 *
 * @details This file exists to verify the Unity test framework
 *          is correctly integrated and CI can run tests.
 *          Real module tests are added from Sprint 2 onwards.
 *
 * @note    To add a real test:
 *          1. Create test_<module_name>.c in tests/
 *          2. Add the file to tests/CMakeLists.txt
 *          3. Add a call to run_<module>_tests() in test_runner.c
 *
 * @author  BaseSync Team
 * @date    2025
 */

/* --- Includes ---------------------------------------------------------------*/
#include "Unity/unity.h"

/* --- Test Functions ----------------------------------------------------
 * Naming convention: test_<module>_<condition>_<expected_result>
 * Every test function MUST start with "test_"
 */

/**
 * @brief Verify Unity framework is operational.
 *
 * This test always passes. Its purpose is to confirm:
 *  1. Unity compiled correctly
 *  2. The test runner links and executes
 *  3. CI can detect passing and failing tests
 */
void test_unity_framework_is_operational(void)
{
    /* TEST_ASSERT_EQUAL(expected, actual) */
    TEST_ASSERT_EQUAL(1, 1);
}

/**
 * @brief Verify basic integer comparison works as expected.
 */
void test_placeholder_integer_assertion(void)
{
    int result = 5 + 3;
    TEST_ASSERT_EQUAL(8, result);
}

/**
 * @brief Verify boolean assertions work.
 */
void test_placeholder_boolean_assertion(void)
{
    int flag = 1;
    TEST_ASSERT_TRUE(flag);
    TEST_ASSERT_FALSE(0);
}
