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
extern void can_setUp(void);
extern void can_tearDown(void);
extern void sm_setUp(void);
extern void sm_tearDown(void);
extern void fm_setUp(void);
extern void fm_tearDown(void);

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

/* ─── Sprint 3: Fault Manager tests ──────────────────────────────────────── */
extern void test_fault_check_null_data_returns_fault_invalid_data(void);
extern void test_fault_check_all_safe_data_returns_fault_none(void);
extern void test_fault_check_batt_temp_below_threshold_no_fault(void);
extern void test_fault_check_batt_temp_at_threshold_no_fault(void);
extern void test_fault_check_batt_temp_above_threshold_sets_bit(void);
extern void test_fault_check_motor_temp_above_threshold_sets_bit(void);
extern void test_fault_check_motor_temp_at_threshold_no_fault(void);
extern void test_fault_check_over_current_above_threshold_sets_bit(void);
extern void test_fault_check_negative_current_no_fault(void);
extern void test_fault_check_under_voltage_below_threshold_sets_bit(void);
extern void test_fault_check_voltage_at_min_threshold_no_fault(void);
extern void test_fault_check_over_voltage_above_threshold_sets_bit(void);
extern void test_fault_check_voltage_at_max_threshold_no_fault(void);
extern void test_fault_check_fault_switch_pressed_sets_manual_trigger(void);
extern void test_fault_check_fault_switch_not_pressed_no_manual_trigger(void);
extern void test_fault_check_multiple_faults_all_bits_set(void);
extern void test_fault_is_set_returns_false_for_clear_bit(void);
extern void test_fault_is_set_returns_true_for_set_bit(void);
extern void test_fault_get_name_returns_none_for_no_fault(void);
extern void test_fault_get_name_returns_correct_name_for_over_temp_batt(void);

/* ─── Sprint 3: EV State Machine tests ───────────────────────────────────── */
extern void test_sm_init_state_is_init(void);
extern void test_sm_get_state_name_after_init_is_init(void);
extern void test_sm_init_to_idle_on_first_run_with_no_fault(void);
extern void test_sm_state_name_is_idle_after_transition(void);
extern void test_sm_idle_to_running_when_throttle_above_deadband(void);
extern void test_sm_stays_idle_when_throttle_at_deadband(void);
extern void test_sm_stays_idle_when_brake_active_even_with_throttle(void);
extern void test_sm_running_to_idle_when_throttle_zero_and_brake(void);
extern void test_sm_stays_running_when_throttle_zero_but_no_brake(void);
extern void test_sm_idle_to_fault_on_fault_code(void);
extern void test_sm_running_to_fault_on_fault_code(void);
extern void test_sm_fault_to_safe_state_on_next_run(void);
extern void test_sm_safe_state_name_correct(void);
extern void test_sm_stays_safe_state_even_with_no_faults(void);
extern void test_sm_null_data_goes_to_safe_state(void);
extern void test_sm_set_fault_from_running_transitions_to_fault(void);
extern void test_sm_set_fault_with_none_is_ignored(void);
extern void test_sm_reset_from_safe_state_returns_ok(void);
extern void test_sm_reset_from_running_returns_error(void);
extern void test_sm_reset_from_idle_returns_error(void);
/* Note: 2 additional SM tests below */
extern void test_sm_running_to_fault_on_fault_code(void);
extern void test_sm_stays_running_when_throttle_zero_but_no_brake(void);

/* ─── Sprint 3: CAN Driver tests ─────────────────────────────────────────── */
extern void test_can_driver_init_returns_ok(void);
extern void test_can_get_last_frame_before_any_send_returns_error(void);
extern void test_can_get_last_frame_null_pointer_returns_invalid(void);
extern void test_can_send_status_before_init_returns_not_ready(void);
extern void test_can_send_status_frame_id_is_0x100(void);
extern void test_can_send_status_byte0_is_ev_state(void);
extern void test_can_send_status_byte1_is_fault_code(void);
extern void test_can_send_status_byte2_is_motor_duty(void);
extern void test_can_send_status_uptime_encoded_little_endian(void);
extern void test_can_send_sensor_pack1_frame_id_is_0x101(void);
extern void test_can_send_sensor_pack1_batt_temp_encoded_x10_little_endian(void);
extern void test_can_send_sensor_pack1_speed_rpm_encoded_correctly(void);
extern void test_can_send_fault_frame_id_is_0x1FF(void);
extern void test_can_send_fault_frame_byte0_is_fault_code(void);
extern void test_can_send_fault_frame_byte1_is_reserved_zero(void);
extern void test_can_send_fault_frame_timestamp_little_endian_in_bytes_2_5(void);

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

    /* ── Sprint 3: Fault Manager ── */
    RUN_TEST(test_fault_check_null_data_returns_fault_invalid_data);
    RUN_TEST(test_fault_check_all_safe_data_returns_fault_none);
    RUN_TEST(test_fault_check_batt_temp_below_threshold_no_fault);
    RUN_TEST(test_fault_check_batt_temp_at_threshold_no_fault);
    RUN_TEST(test_fault_check_batt_temp_above_threshold_sets_bit);
    RUN_TEST(test_fault_check_motor_temp_above_threshold_sets_bit);
    RUN_TEST(test_fault_check_motor_temp_at_threshold_no_fault);
    RUN_TEST(test_fault_check_over_current_above_threshold_sets_bit);
    RUN_TEST(test_fault_check_negative_current_no_fault);
    RUN_TEST(test_fault_check_under_voltage_below_threshold_sets_bit);
    RUN_TEST(test_fault_check_voltage_at_min_threshold_no_fault);
    RUN_TEST(test_fault_check_over_voltage_above_threshold_sets_bit);
    RUN_TEST(test_fault_check_voltage_at_max_threshold_no_fault);
    RUN_TEST(test_fault_check_fault_switch_pressed_sets_manual_trigger);
    RUN_TEST(test_fault_check_fault_switch_not_pressed_no_manual_trigger);
    RUN_TEST(test_fault_check_multiple_faults_all_bits_set);
    RUN_TEST(test_fault_is_set_returns_false_for_clear_bit);
    RUN_TEST(test_fault_is_set_returns_true_for_set_bit);
    RUN_TEST(test_fault_get_name_returns_none_for_no_fault);
    RUN_TEST(test_fault_get_name_returns_correct_name_for_over_temp_batt);

    /* ── Sprint 3: EV State Machine ── */
    RUN_TEST(test_sm_init_state_is_init);
    RUN_TEST(test_sm_get_state_name_after_init_is_init);
    RUN_TEST(test_sm_init_to_idle_on_first_run_with_no_fault);
    RUN_TEST(test_sm_state_name_is_idle_after_transition);
    RUN_TEST(test_sm_idle_to_running_when_throttle_above_deadband);
    RUN_TEST(test_sm_stays_idle_when_throttle_at_deadband);
    RUN_TEST(test_sm_stays_idle_when_brake_active_even_with_throttle);
    RUN_TEST(test_sm_running_to_idle_when_throttle_zero_and_brake);
    RUN_TEST(test_sm_stays_running_when_throttle_zero_but_no_brake);
    RUN_TEST(test_sm_idle_to_fault_on_fault_code);
    RUN_TEST(test_sm_running_to_fault_on_fault_code);
    RUN_TEST(test_sm_fault_to_safe_state_on_next_run);
    RUN_TEST(test_sm_safe_state_name_correct);
    RUN_TEST(test_sm_stays_safe_state_even_with_no_faults);
    RUN_TEST(test_sm_null_data_goes_to_safe_state);
    RUN_TEST(test_sm_set_fault_from_running_transitions_to_fault);
    RUN_TEST(test_sm_set_fault_with_none_is_ignored);
    RUN_TEST(test_sm_reset_from_safe_state_returns_ok);
    RUN_TEST(test_sm_reset_from_running_returns_error);
    RUN_TEST(test_sm_reset_from_idle_returns_error);

    /* ── Sprint 3: CAN Driver ── */
    RUN_TEST(test_can_driver_init_returns_ok);
    RUN_TEST(test_can_get_last_frame_before_any_send_returns_error);
    RUN_TEST(test_can_get_last_frame_null_pointer_returns_invalid);
    RUN_TEST(test_can_send_status_before_init_returns_not_ready);
    RUN_TEST(test_can_send_status_frame_id_is_0x100);
    RUN_TEST(test_can_send_status_byte0_is_ev_state);
    RUN_TEST(test_can_send_status_byte1_is_fault_code);
    RUN_TEST(test_can_send_status_byte2_is_motor_duty);
    RUN_TEST(test_can_send_status_uptime_encoded_little_endian);
    RUN_TEST(test_can_send_sensor_pack1_frame_id_is_0x101);
    RUN_TEST(test_can_send_sensor_pack1_batt_temp_encoded_x10_little_endian);
    RUN_TEST(test_can_send_sensor_pack1_speed_rpm_encoded_correctly);
    RUN_TEST(test_can_send_fault_frame_id_is_0x1FF);
    RUN_TEST(test_can_send_fault_frame_byte0_is_fault_code);
    RUN_TEST(test_can_send_fault_frame_byte1_is_reserved_zero);
    RUN_TEST(test_can_send_fault_frame_timestamp_little_endian_in_bytes_2_5);

    return UNITY_END();
}
