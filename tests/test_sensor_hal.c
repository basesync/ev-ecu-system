/**
 * @file    test_sensor_hal.c
 * @brief   Unit tests for the sensor_hal module
 *
 * @details Tests every public function in sensor_hal.c:
 *          - ADC channel reads (battery temp, motor temp, current, voltage, throttle)
 *          - GPIO reads (brake switch, fault switch) with debounce
 *          - sensor_read_all() integration
 *
 *          Each test follows the Arrange–Act–Assert (AAA) pattern:
 *            Arrange: Set up mock ADC/GPIO values
 *            Act:     Call the function under test
 *            Assert:  Check the result with Unity assertions
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include "Unity/unity.h"
#include "sensor_hal.h"
#include "ev_config.h"
#include "mocks/mock_stm32_hal_adc.h"
#include "mocks/mock_stm32_hal_gpio.h"
#include "mocks/mock_stm32_hal_tim.h"

/* ─── Test fixtures (HAL handles used by sensor_init) ───────────────────── */
static ADC_HandleTypeDef s_test_hadc;
static TIM_HandleTypeDef s_test_htim;

/* ─── setUp and tearDown ─────────────────────────────────────────────────── */

/**
 * @brief Runs before EVERY test function.
 * Resets all mocks and re-initialises sensor_hal.
 */
void sensor_setUp(void)
{
    /* Reset all mock state so tests do not affect each other */
    mock_adc_reset();
    mock_gpio_reset();
    mock_tim_reset();

    /* Reset simulated time to 0 */
    mock_hal_set_tick(0U);

    /* Set up the test HAL handles pointing at mock instances */
    s_test_hadc.Instance = (uint32_t)0x40012400U;  /* ADC1 stub address */
    s_test_htim.Instance = mock_tim_get_instance();

    /* Re-initialise sensor_hal with fresh handles before each test */
    (void)sensor_init(&s_test_hadc, &s_test_htim);
}

void sensor_tearDown(void)
{
    /* Nothing to clean up — mocks are reset in setUp */
}

/* ═══════════════════════════════════════════════════════════════════════════
 * BATTERY TEMPERATURE TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_batt_temp_adc_zero_returns_zero_degrees(void)
{
    /* Arrange: ADC returns 0 → 0V → 0°C */
    mock_adc_set_channel_value(ADC_CHANNEL_0, 0U);

    /* Act */
    float result = sensor_read_batt_temp();

    /* Assert: 0 ADC counts = 0V = 0°C */
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 0.0f, result);
}

void test_sensor_batt_temp_adc_midscale_returns_50_degrees(void)
{
    /*
     * Arrange: ADC = 2048 → voltage ≈ 1.65V → temp = 1650mV / 10mV_per_deg = 165°C
     *
     * Wait — let's recalculate with the actual formula in sensor_hal.c:
     * voltage = (2048 / 4095) * 3.3 = 1.6496V
     * temp = (1.6496 * 1000) / 10.0 = 164.96 ≈ 165°C
     *
     * But our sensor range is 0–100°C, so midscale of the sensor range
     * corresponds to a lower ADC value:
     * For 50°C: voltage = 50 * 10mV = 500mV = 0.5V
     * ADC for 0.5V = (0.5 / 3.3) * 4095 = 620.45 ≈ 620
     */
    mock_adc_set_channel_value(ADC_CHANNEL_0, 620U);

    /* Act */
    float result = sensor_read_batt_temp();

    /* Assert: ADC=620 → ~50°C (±2°C tolerance for integer math rounding) */
    TEST_ASSERT_FLOAT_WITHIN(2.0f, 50.0f, result);
}

void test_sensor_batt_temp_adc_fullscale_returns_max_voltage_temp(void)
{
    /* Arrange: ADC = 4095 → 3.3V → 330°C (LM35 at full ADC range) */
    mock_adc_set_channel_value(ADC_CHANNEL_0, 4095U);

    /* Act */
    float result = sensor_read_batt_temp();

    /* Assert: 3.3V / 10mV_per_deg = 330°C */
    TEST_ASSERT_FLOAT_WITHIN(2.0f, 330.0f, result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * MOTOR TEMPERATURE TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_motor_temp_adc_midscale_returns_same_as_batt_at_same_adc(void)
{
    /*
     * Motor temp uses the same LM35 transfer function as battery temp.
     * At the same ADC value, both should return the same temperature.
     */
    mock_adc_set_channel_value(ADC_CHANNEL_1, 620U);

    float result = sensor_read_motor_temp();

    TEST_ASSERT_FLOAT_WITHIN(2.0f, 50.0f, result);
}

void test_sensor_motor_temp_reads_different_channel_than_batt_temp(void)
{
    /*
     * Set channel 0 (batt temp) to 0, channel 1 (motor temp) to 620.
     * Motor temp must return ~50°C, proving it reads channel 1 not channel 0.
     */
    mock_adc_set_channel_value(ADC_CHANNEL_0, 0U);
    mock_adc_set_channel_value(ADC_CHANNEL_1, 620U);

    float batt_temp  = sensor_read_batt_temp();
    float motor_temp = sensor_read_motor_temp();

    /* Battery temp should be ~0, motor temp should be ~50 */
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 0.0f,  batt_temp);
    TEST_ASSERT_FLOAT_WITHIN(2.0f, 50.0f, motor_temp);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * CURRENT SENSOR TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_current_adc_midscale_returns_zero_amps(void)
{
    /*
     * ACS712 midpoint: 2.5V output = 0A.
     * ADC for 2.5V = (2.5 / 3.3) * 4095 = 3102.27 ≈ 3102
     */
    mock_adc_set_channel_value(ADC_CHANNEL_2, 3102U);

    float result = sensor_read_current();

    /* Should be 0A ± 0.5A tolerance for ADC quantisation */
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 0.0f, result);
}

void test_sensor_current_above_midscale_returns_positive_current(void)
{
    /*
     * ACS712: above 2.5V = positive current (discharge).
     * At 3.0V: current = (3.0 - 2.5) / 0.1 = 5A
     * ADC for 3.0V = (3.0 / 3.3) * 4095 = 3722
     */
    mock_adc_set_channel_value(ADC_CHANNEL_2, 3722U);

    float result = sensor_read_current();

    /* Should be ~+5A */
    TEST_ASSERT_FLOAT_WITHIN(1.0f, 5.0f, result);
}

void test_sensor_current_below_midscale_returns_negative_current(void)
{
    /*
     * ACS712: below 2.5V = negative current (charging/regen).
     * At 2.0V: current = (2.0 - 2.5) / 0.1 = -5A
     * ADC for 2.0V = (2.0 / 3.3) * 4095 = 2482
     */
    mock_adc_set_channel_value(ADC_CHANNEL_2, 2482U);

    float result = sensor_read_current();

    /* Should be ~-5A */
    TEST_ASSERT_FLOAT_WITHIN(1.0f, -5.0f, result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * VOLTAGE SENSOR TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_voltage_adc_zero_returns_zero_volts(void)
{
    mock_adc_set_channel_value(ADC_CHANNEL_3, 0U);

    float result = sensor_read_voltage();

    TEST_ASSERT_FLOAT_WITHIN(0.1f, 0.0f, result);
}

void test_sensor_voltage_adc_fullscale_returns_expected_battery_voltage(void)
{
    /*
     * ADC = 4095 → adc_voltage = 3.3V
     * battery_V = 3.3V / 0.0909 = 36.3V
     * (resistor divider: R1=100k, R2=10k → ratio = 10/110 = 0.0909)
     */
    mock_adc_set_channel_value(ADC_CHANNEL_3, 4095U);

    float result = sensor_read_voltage();

    TEST_ASSERT_FLOAT_WITHIN(0.5f, 36.3f, result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * THROTTLE TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_throttle_adc_zero_returns_zero_percent(void)
{
    mock_adc_set_channel_value(ADC_CHANNEL_4, 0U);

    uint8_t result = sensor_read_throttle();

    TEST_ASSERT_EQUAL_UINT8(0U, result);
}

void test_sensor_throttle_adc_fullscale_returns_100_percent(void)
{
    mock_adc_set_channel_value(ADC_CHANNEL_4, 4095U);

    uint8_t result = sensor_read_throttle();

    TEST_ASSERT_EQUAL_UINT8(100U, result);
}

void test_sensor_throttle_midscale_returns_50_percent(void)
{
    /* ADC = 2048 → (2048 * 100) / 4095 = 50% */
    mock_adc_set_channel_value(ADC_CHANNEL_4, 2048U);

    uint8_t result = sensor_read_throttle();

    /* Accept 49 or 50 — integer division may round down */
    TEST_ASSERT_TRUE((result == 49U) || (result == 50U));
}

void test_sensor_throttle_never_exceeds_100_percent(void)
{
    /*
     * Inject a value slightly above 4095 by mocking 4095 max —
     * the clamp in sensor_read_throttle() must catch any edge case.
     */
    mock_adc_set_channel_value(ADC_CHANNEL_4, 4095U);

    uint8_t result = sensor_read_throttle();

    TEST_ASSERT_TRUE(result <= 100U);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * BRAKE AND FAULT SWITCH TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_brake_gpio_low_returns_true_after_debounce(void)
{
    /*
     * Active-low wiring: PB0 LOW = switch pressed = return true.
     * We must advance simulated time past the debounce period.
     */
    mock_gpio_set_pin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);  /* LOW = pressed */

    /* First read — state just changed, debounce not yet passed */
    mock_hal_set_tick(0U);
    TEST_ASSERT_FALSE(sensor_read_brake());

    /* Advance time past debounce period */
    mock_hal_set_tick(EV_GPIO_DEBOUNCE_MS + 5U);
    bool after_debounce = sensor_read_brake();

    /* After debounce, should be true (pressed) */
    TEST_ASSERT_TRUE(after_debounce);
}

void test_sensor_brake_gpio_high_returns_false(void)
{
    /* 1. First, set the brake to an ACTIVE state so we can test it turning OFF */
    mock_gpio_set_pin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET); // Pressed
    mock_hal_set_tick(100);
    (void)sensor_read_brake(); // Latch to TRUE

    /* 2. Act: Release the switch (HIGH) */
    mock_gpio_set_pin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);   /* HIGH = released */

    /* 3. MNC FIX: Advance virtual time past the debounce period (e.g., 50ms) */
    mock_hal_set_tick(100 + EV_GPIO_DEBOUNCE_MS + 5U);

    /* 4. Assert: Result should now be FALSE */
    TEST_ASSERT_FALSE(sensor_read_brake());
}

void test_sensor_fault_switch_gpio_low_returns_true_after_debounce(void)
{
    mock_gpio_set_pin(GPIOB, GPIO_PIN_1, GPIO_PIN_RESET);
    mock_hal_set_tick(0U);
    (void)sensor_read_fault_switch();   /* First read — start debounce */
    mock_hal_set_tick(EV_GPIO_DEBOUNCE_MS + 5U);

    bool result = sensor_read_fault_switch();

    TEST_ASSERT_TRUE(result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * sensor_init() TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_init_null_adc_handle_returns_invalid(void)
{
    ev_status_t result = sensor_init(NULL, &s_test_htim);

    TEST_ASSERT_EQUAL(EV_STATUS_INVALID, result);
}

void test_sensor_init_null_tim_handle_returns_invalid(void)
{
    ev_status_t result = sensor_init(&s_test_hadc, NULL);

    TEST_ASSERT_EQUAL(EV_STATUS_INVALID, result);
}

void test_sensor_init_valid_handles_returns_ok(void)
{
    ev_status_t result = sensor_init(&s_test_hadc, &s_test_htim);

    TEST_ASSERT_EQUAL(EV_STATUS_OK, result);
}

/* ═══════════════════════════════════════════════════════════════════════════
 * sensor_read_all() TESTS
 * ═══════════════════════════════════════════════════════════════════════════ */

void test_sensor_read_all_null_pointer_returns_invalid(void)
{
    ev_status_t result = sensor_read_all(NULL);

    TEST_ASSERT_EQUAL(EV_STATUS_INVALID, result);
}

void test_sensor_read_all_valid_pointer_populates_all_fields(void)
{
    sensor_data_t data;

    /* Arrange: set all ADC channels to known values */
    mock_adc_set_channel_value(ADC_CHANNEL_0, 620U);   /* ~50°C battery temp  */
    mock_adc_set_channel_value(ADC_CHANNEL_1, 620U);   /* ~50°C motor temp    */
    mock_adc_set_channel_value(ADC_CHANNEL_2, 3102U);  /* ~0A current         */
    mock_adc_set_channel_value(ADC_CHANNEL_3, 0U);     /* 0V voltage          */
    mock_adc_set_channel_value(ADC_CHANNEL_4, 0U);     /* 0% throttle         */
    mock_gpio_set_pin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);  /* Brake released     */
    mock_gpio_set_pin(GPIOB, GPIO_PIN_1, GPIO_PIN_SET);  /* Fault sw released  */

    /* Advance time to ensure the 'Released' state is processed by the debouncer */
    mock_hal_set_tick(500);

    /* Act */
    ev_status_t status = sensor_read_all(&data);

    /* Assert: function returned OK and fields are populated */
    TEST_ASSERT_EQUAL(EV_STATUS_OK, status);
    TEST_ASSERT_FLOAT_WITHIN(2.0f, 50.0f, data.batt_temp_c);
    TEST_ASSERT_FLOAT_WITHIN(2.0f, 50.0f, data.motor_temp_c);
    TEST_ASSERT_FLOAT_WITHIN(0.5f, 0.0f,  data.current_a);
    TEST_ASSERT_EQUAL_UINT8(0U, data.throttle_pct);
    TEST_ASSERT_FALSE(data.brake_active);
    TEST_ASSERT_FALSE(data.fault_switch);
}
