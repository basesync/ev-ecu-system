/**
 * @file    test_runner.c
 * @brief   Unity test runner — Sprint 2 (Sensor HAL + Motor Control tests)
 */

#include "Unity/unity.h"

/* ─── Module setUp/tearDown declared here ─────────────────────────────────── */
extern void motor_setUp(void);
extern void motor_tearDown(void);
extern void sensor_setUp(void);
extern void sensor_tearDown(void);
extern void protocol_setUp(void);
extern void protocol_tearDown(void);

/* Track which module is active so setUp/tearDown dispatch correctly */
typedef enum { MODULE_NONE, MODULE_SENSOR, MODULE_MOTOR, MODULR_PROTOCOL } active_module_t;
static active_module_t s_active_module = MODULE_NONE;

void setUp(void)
{
    if      (s_active_module == MODULE_SENSOR) { sensor_setUp(); }
    else if (s_active_module == MODULE_MOTOR)  { motor_setUp();  }
    else if (s_active_module == MODULR_PROTOCOL)  { protocol_setUp();  }
}

void tearDown(void)
{
    if      (s_active_module == MODULE_SENSOR) { sensor_tearDown(); }
    else if (s_active_module == MODULE_MOTOR)  { motor_tearDown();  }
    else if (s_active_module == MODULR_PROTOCOL)  { protocol_tearDown();  }
}

/* ─── Sprint 1: Placeholder tests ────────────────────────────────────────── */
extern void test_unity_framework_is_operational(void);
extern void test_placeholder_integer_assertion(void);
extern void test_placeholder_boolean_assertion(void);

/* ─── Sprint 2: Sensor HAL tests ─────────────────────────────────────────── */
extern void test_sensor_batt_temp_adc_zero_returns_zero_degrees(void);
extern void test_sensor_batt_temp_adc_midscale_returns_50_degrees(void);
extern void test_sensor_batt_temp_adc_fullscale_returns_max_voltage_temp(void);
extern void test_sensor_motor_temp_adc_midscale_returns_same_as_batt_at_same_adc(void);
extern void test_sensor_motor_temp_reads_different_channel_than_batt_temp(void);
extern void test_sensor_current_adc_midscale_returns_zero_amps(void);
extern void test_sensor_current_above_midscale_returns_positive_current(void);
extern void test_sensor_current_below_midscale_returns_negative_current(void);
extern void test_sensor_voltage_adc_zero_returns_zero_volts(void);
extern void test_sensor_voltage_adc_fullscale_returns_expected_battery_voltage(void);
extern void test_sensor_throttle_adc_zero_returns_zero_percent(void);
extern void test_sensor_throttle_adc_fullscale_returns_100_percent(void);
extern void test_sensor_throttle_midscale_returns_50_percent(void);
extern void test_sensor_throttle_never_exceeds_100_percent(void);
extern void test_sensor_brake_gpio_low_returns_true_after_debounce(void);
extern void test_sensor_brake_gpio_high_returns_false(void);
extern void test_sensor_fault_switch_gpio_low_returns_true_after_debounce(void);
extern void test_sensor_init_null_adc_handle_returns_invalid(void);
extern void test_sensor_init_null_tim_handle_returns_invalid(void);
extern void test_sensor_init_valid_handles_returns_ok(void);
extern void test_sensor_read_all_null_pointer_returns_invalid(void);
extern void test_sensor_read_all_valid_pointer_populates_all_fields(void);

/* ─── Sprint 2: Motor Control tests ──────────────────────────────────────── */
extern void test_motor_init_null_handle_returns_invalid(void);
extern void test_motor_init_valid_handle_returns_ok(void);
extern void test_motor_init_sets_initial_speed_to_zero(void);
extern void test_motor_init_pwm_start_error_returns_hal_error(void);
extern void test_motor_set_speed_zero_sets_duty_zero(void);
extern void test_motor_set_speed_100_sets_duty_full(void);
extern void test_motor_set_speed_50pct_sets_half_duty(void);
extern void test_motor_set_speed_over_100_returns_invalid(void);
extern void test_motor_set_speed_over_100_does_not_change_speed(void);
extern void test_motor_set_speed_returns_ok_for_valid_values(void);
extern void test_motor_set_speed_below_deadband_sets_zero(void);
extern void test_motor_stop_sets_duty_to_zero(void);
extern void test_motor_stop_always_returns_ok(void);
extern void test_motor_stop_from_zero_still_returns_ok(void);
extern void test_motor_get_speed_returns_last_set_value(void);
extern void test_motor_get_speed_returns_zero_after_stop(void);
extern void test_motor_brake_override_throttle_80pct_with_brake_gives_zero(void);
extern void test_motor_brake_not_active_throttle_sets_speed(void);

/* ─── Sprint 2: Protocol Prep tests (S2-11) ──────────────────────────────── */
extern void test_fault_logger_init_with_null_handle_returns_ok(void);
extern void test_fault_logger_init_with_valid_handle_returns_ok(void);
extern void test_fault_logger_write_before_init_returns_not_ready(void);
extern void test_fault_logger_write_returns_ok_after_init(void);
extern void test_fault_logger_write_with_zero_fault_code_returns_ok(void);
extern void test_fault_logger_write_with_all_faults_returns_ok(void);
extern void test_fault_logger_read_last_null_code_returns_invalid(void);
extern void test_fault_logger_read_last_null_timestamp_returns_invalid(void);
extern void test_fault_logger_read_last_stub_returns_error_empty_log(void);
extern void test_fault_logger_get_count_returns_zero_in_stub_mode(void);
extern void test_fault_logger_clear_all_returns_ok_after_init(void);
extern void test_fault_logger_backend_status_is_stub_in_sprint2(void);
extern void test_fault_logger_backend_status_still_stub_with_handle(void);
extern void test_sensor_enable_i2c_temp_null_stays_in_adc_mode(void);
extern void test_sensor_enable_i2c_temp_valid_handle_returns_ok(void);
extern void test_sensor_get_temp_backend_returns_stub_in_sprint2(void);
extern void test_sensor_get_temp_backend_still_stub_after_enabling_i2c(void);
extern void test_sensor_read_batt_temp_still_works_after_i2c_enable(void);
extern void test_fault_log_entry_struct_is_8_bytes(void);
extern void test_fault_log_entry_fault_code_is_first_byte(void);
extern void test_fault_log_entry_timestamp_is_last_4_bytes(void);
extern void test_uart_baud_rate_is_115200(void);
extern void test_i2c_tmp102_address_is_valid_7bit(void);
extern void test_i2c_tmp102_8bit_address_is_7bit_shifted(void);
extern void test_spi_flash_log_max_entries_matches_area_size(void);
extern void test_spi_flash_sector_size_divides_log_size_evenly(void);

int main(void)
{
    UNITY_BEGIN();

    /* ── Sprint 1: Framework verification (no module setup needed) ── */
    s_active_module = MODULE_NONE;
    RUN_TEST(test_unity_framework_is_operational);
    RUN_TEST(test_placeholder_integer_assertion);
    RUN_TEST(test_placeholder_boolean_assertion);

    /* ── Sprint 2: Sensor HAL ── */
    s_active_module = MODULE_SENSOR;
    RUN_TEST(test_sensor_batt_temp_adc_zero_returns_zero_degrees);
    RUN_TEST(test_sensor_batt_temp_adc_midscale_returns_50_degrees);
    RUN_TEST(test_sensor_batt_temp_adc_fullscale_returns_max_voltage_temp);
    RUN_TEST(test_sensor_motor_temp_adc_midscale_returns_same_as_batt_at_same_adc);
    RUN_TEST(test_sensor_motor_temp_reads_different_channel_than_batt_temp);
    RUN_TEST(test_sensor_current_adc_midscale_returns_zero_amps);
    RUN_TEST(test_sensor_current_above_midscale_returns_positive_current);
    RUN_TEST(test_sensor_current_below_midscale_returns_negative_current);
    RUN_TEST(test_sensor_voltage_adc_zero_returns_zero_volts);
    RUN_TEST(test_sensor_voltage_adc_fullscale_returns_expected_battery_voltage);
    RUN_TEST(test_sensor_throttle_adc_zero_returns_zero_percent);
    RUN_TEST(test_sensor_throttle_adc_fullscale_returns_100_percent);
    RUN_TEST(test_sensor_throttle_midscale_returns_50_percent);
    RUN_TEST(test_sensor_throttle_never_exceeds_100_percent);
    RUN_TEST(test_sensor_brake_gpio_low_returns_true_after_debounce);
    RUN_TEST(test_sensor_brake_gpio_high_returns_false);
    RUN_TEST(test_sensor_fault_switch_gpio_low_returns_true_after_debounce);
    RUN_TEST(test_sensor_init_null_adc_handle_returns_invalid);
    RUN_TEST(test_sensor_init_null_tim_handle_returns_invalid);
    RUN_TEST(test_sensor_init_valid_handles_returns_ok);
    RUN_TEST(test_sensor_read_all_null_pointer_returns_invalid);
    RUN_TEST(test_sensor_read_all_valid_pointer_populates_all_fields);

    /* ── Sprint 2: Motor Control ── */
    s_active_module = MODULE_MOTOR;
    RUN_TEST(test_motor_init_null_handle_returns_invalid);
    RUN_TEST(test_motor_init_valid_handle_returns_ok);
    RUN_TEST(test_motor_init_sets_initial_speed_to_zero);
    RUN_TEST(test_motor_init_pwm_start_error_returns_hal_error);
    RUN_TEST(test_motor_set_speed_zero_sets_duty_zero);
    RUN_TEST(test_motor_set_speed_100_sets_duty_full);
    RUN_TEST(test_motor_set_speed_50pct_sets_half_duty);
    RUN_TEST(test_motor_set_speed_over_100_returns_invalid);
    RUN_TEST(test_motor_set_speed_over_100_does_not_change_speed);
    RUN_TEST(test_motor_set_speed_returns_ok_for_valid_values);
    RUN_TEST(test_motor_set_speed_below_deadband_sets_zero);
    RUN_TEST(test_motor_stop_sets_duty_to_zero);
    RUN_TEST(test_motor_stop_always_returns_ok);
    RUN_TEST(test_motor_stop_from_zero_still_returns_ok);
    RUN_TEST(test_motor_get_speed_returns_last_set_value);
    RUN_TEST(test_motor_get_speed_returns_zero_after_stop);
    RUN_TEST(test_motor_brake_override_throttle_80pct_with_brake_gives_zero);
    RUN_TEST(test_motor_brake_not_active_throttle_sets_speed);

    /* ── Sprint 2: Protocol Prep (S2-11) ── */
    /* fault_logger stub */
    RUN_TEST(test_fault_logger_init_with_null_handle_returns_ok);
    RUN_TEST(test_fault_logger_init_with_valid_handle_returns_ok);
    RUN_TEST(test_fault_logger_write_before_init_returns_not_ready);
    RUN_TEST(test_fault_logger_write_returns_ok_after_init);
    RUN_TEST(test_fault_logger_write_with_zero_fault_code_returns_ok);
    RUN_TEST(test_fault_logger_write_with_all_faults_returns_ok);
    RUN_TEST(test_fault_logger_read_last_null_code_returns_invalid);
    RUN_TEST(test_fault_logger_read_last_null_timestamp_returns_invalid);
    RUN_TEST(test_fault_logger_read_last_stub_returns_error_empty_log);
    RUN_TEST(test_fault_logger_get_count_returns_zero_in_stub_mode);
    RUN_TEST(test_fault_logger_clear_all_returns_ok_after_init);
    RUN_TEST(test_fault_logger_backend_status_is_stub_in_sprint2);
    RUN_TEST(test_fault_logger_backend_status_still_stub_with_handle);
    /* sensor_hal I2C stubs */
    RUN_TEST(test_sensor_enable_i2c_temp_null_stays_in_adc_mode);
    RUN_TEST(test_sensor_enable_i2c_temp_valid_handle_returns_ok);
    RUN_TEST(test_sensor_get_temp_backend_returns_stub_in_sprint2);
    RUN_TEST(test_sensor_get_temp_backend_still_stub_after_enabling_i2c);
    RUN_TEST(test_sensor_read_batt_temp_still_works_after_i2c_enable);
    /* struct layout */
    RUN_TEST(test_fault_log_entry_struct_is_8_bytes);
    RUN_TEST(test_fault_log_entry_fault_code_is_first_byte);
    RUN_TEST(test_fault_log_entry_timestamp_is_last_4_bytes);
    /* config constants */
    RUN_TEST(test_uart_baud_rate_is_115200);
    RUN_TEST(test_i2c_tmp102_address_is_valid_7bit);
    RUN_TEST(test_i2c_tmp102_8bit_address_is_7bit_shifted);
    RUN_TEST(test_spi_flash_log_max_entries_matches_area_size);
    RUN_TEST(test_spi_flash_sector_size_divides_log_size_evenly);

    return UNITY_END();
}
