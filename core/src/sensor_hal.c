/**
 * @file    sensor_hal.c
 * @brief   Sensor HAL implementation — reads all EV ECU sensors
 *
 * @details This file implements the functions declared in sensor_hal.h.
 *          It reads 5 ADC channels, 1 encoder timer, and 2 GPIO pins.
 *
 *          All hardware access goes through the STM32 HAL functions.
 *          This makes it easy to:
 *            1. Test with mocks (unit tests run on a PC)
 *            2. Port to a different STM32 (only change the init, not the logic)
 *
 *          Sensor wiring (from ev_config.h and hardware design doc):
 *            PA0 → ADC1 CH0 → Battery temperature (LM35)
 *            PA1 → ADC1 CH1 → Motor temperature (LM35)
 *            PA2 → ADC1 CH2 → Battery current (ACS712-20A)
 *            PA3 → ADC1 CH3 → Battery voltage (resistor divider)
 *            PA4 → ADC1 CH4 → Throttle potentiometer
 *            PA6 → TIM3 CH1 → Encoder channel A
 *            PA7 → TIM3 CH2 → Encoder channel B
 *            PB0 → GPIO IN  → Brake switch (active low)
 *            PB1 → GPIO IN  → Fault trigger switch (active low)
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include "sensor_hal.h"
#include "ev_config.h"

/* ─── Private Variables ──────────────────────────────────────────────────── */

/* HAL handles stored at initialisation, used by all read functions */
/* 'static' means these are only visible inside this file (good practice) */
static ADC_HandleTypeDef *s_hadc1 = NULL;
static TIM_HandleTypeDef *s_htim3 = NULL;

/* Flag to track whether sensor_init() has been called successfully */
static bool s_initialised = false;

/* I2C handle for TMP102 temperature sensor (Sprint 5) — NULL = ADC simulation */
static I2C_HandleTypeDef *s_hi2c1           = NULL;
static ev_proto_status_t  s_temp_backend    = EV_PROTO_STATUS_STUB;

/* Previous brake and fault switch states for debounce */
static GPIO_PinState s_brake_prev_state      = GPIO_PIN_SET;   /* HIGH = released */
static GPIO_PinState s_fault_sw_prev_state   = GPIO_PIN_SET;
static uint32_t      s_brake_debounce_tick   = 0U;
static uint32_t      s_fault_sw_debounce_tick = 0U;
static bool          s_brake_stable          = false;
static bool          s_fault_sw_stable       = false;

/* ─── Private Function Declarations ─────────────────────────────────────── */
static uint32_t  priv_adc_read_channel(uint32_t channel);
static float     priv_adc_to_voltage(uint32_t raw_adc);
static bool      priv_read_gpio_debounced(GPIO_TypeDef   *port,
                                           uint16_t        pin,
                                           GPIO_PinState  *prev_state,
                                           uint32_t       *debounce_tick,
                                           bool           *stable_state);

/* ─── Public Function Implementations ───────────────────────────────────── */

/**
 * @brief Initialise the sensor HAL module.
 */
ev_status_t sensor_init(ADC_HandleTypeDef *hadc, TIM_HandleTypeDef *htim)
{
    /*
     * Validate input pointers.
     * If either handle is NULL, we cannot read sensors.
     * This catches mistakes like calling sensor_init(NULL, &htim3).
     */
    if (hadc == NULL || htim == NULL)
    {
        return EV_STATUS_INVALID;
    }

    s_brake_prev_state      = GPIO_PIN_SET;   /* Reset to HIGH (released) */
    s_fault_sw_prev_state   = GPIO_PIN_SET;
    s_brake_debounce_tick   = 0U;
    s_fault_sw_debounce_tick = 0U;
    s_brake_stable          = false;
    s_fault_sw_stable       = false;

    /* Store handles for use by all read functions */
    s_hadc1 = hadc;
    s_htim3 = htim;

    /* Start encoder interface — TIM3 will count encoder pulses from now on */
    if (HAL_TIM_Encoder_Start(s_htim3, TIM_CHANNEL_1) != HAL_OK)
    {
        /* Encoder start failed — return error but don't block other sensors */
        return EV_STATUS_HAL_ERROR;
    }

    /* Mark module as ready */
    s_initialised = true;

    return EV_STATUS_OK;
}

/**
 * @brief Read all sensors and populate a sensor_data_t structure.
 */
ev_status_t sensor_read_all(sensor_data_t *data)
{
    /* Guard: caller must pass a valid pointer */
    if (data == NULL)
    {
        return EV_STATUS_INVALID;
    }

    /* Guard: sensor_init() must have been called first */
    if (s_initialised == false)
    {
        return EV_STATUS_NOT_READY;
    }

    /*
     * Read each sensor into the struct.
     * We read all fields even if one fails, so the caller always gets
     * the most complete picture possible.
     *
     * NOTE: We use a local 'status' variable to track if anything failed.
     * A single failed read still populates the rest of the struct.
     */
    ev_status_t status = EV_STATUS_OK;

    data->batt_temp_c  = sensor_read_batt_temp();
    data->motor_temp_c = sensor_read_motor_temp();
    data->current_a    = sensor_read_current();
    data->voltage_v    = sensor_read_voltage();
    data->speed_rpm    = sensor_read_speed();
    data->throttle_pct = sensor_read_throttle();
    data->brake_active = sensor_read_brake();
    data->fault_switch = sensor_read_fault_switch();

    /*
     * Basic sanity check: if all ADC values came back as exactly 0.0,
     * it is likely the ADC handle was not properly configured.
     * This is a heuristic — real hardware should never have all zeros simultaneously.
     */
    if ((data->batt_temp_c == 0.0f) &&
        (data->motor_temp_c == 0.0f) &&
        (data->voltage_v == 0.0f))
    {
        /* Possibly an ADC issue — caller should be aware */
        status = EV_STATUS_ERROR;
    }

    return status;
}

/**
 * @brief Read battery temperature from ADC channel 0.
 */
float sensor_read_batt_temp(void)
{
    uint32_t raw_adc;
    float    voltage_v;
    float    temperature_c;

    /* Read the raw ADC value (0–4095 for 12-bit ADC) */
    raw_adc = priv_adc_read_channel(ADC_CHANNEL_0);

    /* Convert ADC count to voltage in Volts */
    voltage_v = priv_adc_to_voltage(raw_adc);

    /*
     * LM35 temperature sensor transfer function:
     *   Output: 10mV per degree Celsius
     *   So: temperature = voltage / 10mV_per_degree
     *   Example: 350mV → 35.0°C
     *
     * EV_TEMP_SENSOR_MV_PER_DEG = 10.0f (from ev_config.h)
     * We convert voltage from V to mV by multiplying by 1000.
     */
    temperature_c = (voltage_v * 1000.0f) / EV_TEMP_SENSOR_MV_PER_DEG;

    return temperature_c;
}

/**
 * @brief Read motor temperature from ADC channel 1.
 */
float sensor_read_motor_temp(void)
{
    uint32_t raw_adc;
    float    voltage_v;
    float    temperature_c;

    raw_adc = priv_adc_read_channel(ADC_CHANNEL_1);
    voltage_v = priv_adc_to_voltage(raw_adc);

    /* Same LM35 transfer function as battery temperature */
    temperature_c = (voltage_v * 1000.0f) / EV_TEMP_SENSOR_MV_PER_DEG;

    return temperature_c;
}

/**
 * @brief Read battery current from ADC channel 2.
 */
float sensor_read_current(void)
{
    uint32_t raw_adc;
    float    voltage_v;
    float    current_a;

    raw_adc = priv_adc_read_channel(ADC_CHANNEL_2);
    voltage_v = priv_adc_to_voltage(raw_adc);

    /*
     * ACS712-20A current sensor transfer function:
     *   At 0 Amps: output = 2.5V (EV_CURRENT_SENSOR_MIDPOINT_MV / 1000)
     *   Sensitivity: 100mV per Amp (EV_CURRENT_SENSOR_MV_PER_AMP)
     *
     *   current = (voltage - 2.5) / 0.1
     *
     * Positive: motor is drawing current (discharging battery)
     * Negative: regen braking is charging the battery
     */
    float midpoint_v = (float)EV_CURRENT_SENSOR_MIDPOINT_MV / 1000.0f;
    float sensitivity_v_per_a = EV_CURRENT_SENSOR_MV_PER_AMP / 1000.0f;

    current_a = (voltage_v - midpoint_v) / sensitivity_v_per_a;

    return current_a;
}

/**
 * @brief Read battery pack voltage from ADC channel 3.
 */
float sensor_read_voltage(void)
{
    uint32_t raw_adc;
    float    adc_voltage_v;
    float    battery_voltage_v;

    raw_adc = priv_adc_read_channel(ADC_CHANNEL_3);
    adc_voltage_v = priv_adc_to_voltage(raw_adc);

    /*
     * Resistor voltage divider reversal:
     *   The divider scaled the battery voltage DOWN by a factor of
     *   EV_VOLTAGE_DIVIDER_RATIO (R2/(R1+R2) = 10/(100+10) ≈ 0.0909)
     *
     *   To get the real battery voltage, we divide the measured ADC voltage
     *   by the divider ratio:
     *   battery_V = adc_voltage_V / EV_VOLTAGE_DIVIDER_RATIO
     *             = adc_voltage_V / 0.0909
     *             = adc_voltage_V * 11.0
     */
    battery_voltage_v = adc_voltage_v / EV_VOLTAGE_DIVIDER_RATIO;

    return battery_voltage_v;
}

/**
 * @brief Read motor speed from TIM3 encoder.
 */
uint16_t sensor_read_speed(void)
{
    uint32_t encoder_count;
    uint16_t speed_rpm;

    /* Guard: timer must be initialised */
    if (s_htim3 == NULL)
    {
        return 0U;
    }

    /*
     * Read the current encoder counter value.
     * TIM3 in encoder mode counts up/down with each encoder pulse.
     * We read it, calculate RPM, then reset the counter.
     *
     * RPM calculation:
     *   counts_per_rev = EV_ENCODER_PPR (from ev_config.h)
     *   period = EV_SENSOR_SPEED_PERIOD_MS / 1000.0 seconds
     *   revolutions = encoder_count / counts_per_rev
     *   RPM = revolutions / period * 60
     *       = (encoder_count / PPR) * (1000 / period_ms) * 60
     */
    encoder_count = (uint32_t)__HAL_TIM_GET_COUNTER(s_htim3);

    /* Reset counter for next measurement period */
    s_htim3->Instance->CNT = 0U;

    /*
     * Convert count to RPM.
     * Cast to float for division, then back to uint16_t.
     * Limit to uint16_t max to prevent overflow.
     */
    float rpm_f = ((float)encoder_count / (float)EV_ENCODER_PPR)
                  * (1000.0f / (float)EV_SENSOR_SPEED_PERIOD_MS)
                  * 60.0f;

    /* Clamp to uint16_t range */
    if (rpm_f > 65535.0f)
    {
        speed_rpm = 65535U;
    }
    else
    {
        speed_rpm = (uint16_t)rpm_f;
    }

    return speed_rpm;
}

/**
 * @brief Read throttle position from ADC channel 4.
 */
uint8_t sensor_read_throttle(void)
{
    uint32_t raw_adc;
    uint8_t  throttle_pct;

    raw_adc = priv_adc_read_channel(ADC_CHANNEL_4);

    /*
     * Linear mapping: 0 → 0%, 4095 → 100%
     * Formula: pct = (raw / 4095) * 100
     *
     * Integer arithmetic version to avoid float:
     * pct = (raw * 100U) / EV_ADC_MAX_VALUE
     */
    throttle_pct = (uint8_t)((raw_adc * 100U) / EV_ADC_MAX_VALUE);

    /* Safety clamp: never return more than 100 */
    if (throttle_pct > EV_MOTOR_MAX_DUTY_PCT)
    {
        throttle_pct = EV_MOTOR_MAX_DUTY_PCT;
    }

    return throttle_pct;
}

/**
 * @brief Read brake switch state from GPIO PB0.
 */
bool sensor_read_brake(void)
{
    return priv_read_gpio_debounced(GPIOB,
                                    GPIO_PIN_0,
                                    &s_brake_prev_state,
                                    &s_brake_debounce_tick,
                                    &s_brake_stable);
}

/**
 * @brief Read manual fault trigger switch from GPIO PB1.
 */
bool sensor_read_fault_switch(void)
{
    return priv_read_gpio_debounced(GPIOB,
                                    GPIO_PIN_1,
                                    &s_fault_sw_prev_state,
                                    &s_fault_sw_debounce_tick,
                                    &s_fault_sw_stable);
}

/* ─── Private Function Implementations ──────────────────────────────────── */

/**
 * @brief  Read one ADC channel by selecting it then triggering a conversion.
 *
 * @details This function selects the requested channel, starts one conversion
 *          in polling mode, waits for it to complete, and returns the value.
 *
 *          We use polling (not DMA/interrupt) for Sprint 2 because it is
 *          simple and easy to understand. DMA will be considered in Sprint 5.
 *
 * @param  channel  ADC channel number (ADC_CHANNEL_0 through ADC_CHANNEL_4)
 * @retval 12-bit ADC result (0–4095), or 0U on HAL error
 */
static uint32_t priv_adc_read_channel(uint32_t channel)
{
    ADC_ChannelConfTypeDef channel_config;
    uint32_t               adc_value = 0U;

    /* Guard: ADC must be initialised */
    if (s_hadc1 == NULL)
    {
        return 0U;
    }

    /*
     * Configure the channel we want to sample.
     * We must do this before each conversion because we share one ADC
     * across all 5 channels (single-channel sequential reads).
     */
    channel_config.Channel      = channel;
    channel_config.Rank         = ADC_RANK_1;
    channel_config.SamplingTime = ADC_SAMPLETIME_239CYCLES_5;

    if (HAL_ADC_ConfigChannel(s_hadc1, &channel_config) != HAL_OK)
    {
        /* Channel configuration failed — return 0 (safe default) */
        return 0U;
    }

    /* Start a single conversion */
    if (HAL_ADC_Start(s_hadc1) != HAL_OK)
    {
        return 0U;
    }

    /*
     * Wait for conversion to complete.
     * HAL_MAX_DELAY means "wait forever" — in practice the ADC is fast.
     * Sprint 5: replace with a timeout value and handle timeout gracefully.
     */
    if (HAL_ADC_PollForConversion(s_hadc1, HAL_MAX_DELAY) != HAL_OK)
    {
        return 0U;
    }

    /* Read the 12-bit conversion result */
    adc_value = HAL_ADC_GetValue(s_hadc1);

    /* Stop the ADC to prepare for the next channel selection */
    (void)HAL_ADC_Stop(s_hadc1);

    return adc_value;
}

/**
 * @brief  Convert a raw 12-bit ADC value to a voltage in Volts.
 *
 * @details Uses the ADC reference voltage and resolution from ev_config.h.
 *          formula: voltage_V = (raw / ADC_MAX_VALUE) * (VREF_mV / 1000.0)
 *
 * @param  raw_adc  12-bit ADC result (0–4095)
 * @retval Voltage in Volts (0.0 to 3.3V)
 */
static float priv_adc_to_voltage(uint32_t raw_adc)
{
    float voltage_v;

    /*
     * EV_ADC_MAX_VALUE = 4095 (12-bit)
     * EV_ADC_VREF_MV   = 3300 (3.3V reference)
     */
    voltage_v = ((float)raw_adc / (float)EV_ADC_MAX_VALUE)
                * ((float)EV_ADC_VREF_MV / 1000.0f);

    return voltage_v;
}

/**
 * @brief  Read a GPIO pin with software debounce.
 *
 * @details Switch contacts physically "bounce" when pressed, creating rapid
 *          HIGH/LOW transitions before settling. Without debounce, one press
 *          can look like 10–20 presses to the firmware.
 *
 *          Debounce algorithm:
 *            1. Read the current GPIO state
 *            2. If it changed from the previous state, record the time
 *            3. Only update the stable output if the state has held stable
 *               for EV_GPIO_DEBOUNCE_MS milliseconds
 *
 * @param  port          GPIO port (GPIOB)
 * @param  pin           GPIO pin number
 * @param  prev_state    Pointer to previous raw state (persists between calls)
 * @param  debounce_tick Pointer to debounce timestamp (persists between calls)
 * @param  stable_state  Pointer to debounced output state (persists between calls)
 *
 * @retval Debounced switch state:
 *         true  = switch pressed (GPIO LOW, active-low wiring)
 *         false = switch released (GPIO HIGH)
 */
static bool priv_read_gpio_debounced(GPIO_TypeDef  *port,
                                      uint16_t       pin,
                                      GPIO_PinState *prev_state,
                                      uint32_t      *debounce_tick,
                                      bool          *stable_state)
{
    GPIO_PinState current_state = HAL_GPIO_ReadPin(port, pin);

    if (current_state != *prev_state)
    {
        /*
         * State has changed — start (or restart) the debounce timer.
         * We record when the change happened.
         */
        *prev_state    = current_state;
        *debounce_tick = HAL_GetTick();
    }
    else
    {
        /*
         * State is the same as last read.
         * Check if it has been stable for long enough.
         */
        uint32_t elapsed_ms = HAL_GetTick() - *debounce_tick;

        if (elapsed_ms >= EV_GPIO_DEBOUNCE_MS)
        {
            /*
             * State has been stable for EV_GPIO_DEBOUNCE_MS.
             * Accept it as the true switch state.
             *
             * Active-low conversion:
             *   GPIO_PIN_RESET (LOW)  → switch pressed  → return true
             *   GPIO_PIN_SET   (HIGH) → switch released → return false
             */
            *stable_state = (current_state == GPIO_PIN_RESET);
        }
    }

    return *stable_state;
}

/* ─── Sprint 5 Stub Implementations ─────────────────────────────────────── */

/**
 * @brief Enable I2C battery temperature sensing — STUB (Sprint 2–4).
 *
 * @details Sprint 5 TODO: Replace stub body with:
 *   1. Validate hi2c is not NULL
 *   2. Call HAL_I2C_IsDeviceReady(hi2c, EV_I2C_TMP102_ADDR_8BIT, 3, 10)
 *   3. If device responds: store handle, set s_temp_backend = ACTIVE
 *   4. If no response: return EV_STATUS_INVALID (keep ADC fallback)
 */
ev_status_t sensor_enable_i2c_temp(I2C_HandleTypeDef *hi2c)
{
    /*
     * Sprint 2–4 stub: Accept any handle (including NULL).
     * NULL = stay in ADC simulation mode.
     * Non-NULL stored but not used until Sprint 5 implementation.
     */
    s_hi2c1 = hi2c;

    if (hi2c == NULL)
    {
        /* NULL explicitly passed — caller wants ADC simulation */
        s_temp_backend = EV_PROTO_STATUS_STUB;
    }
    else
    {
        /*
         * Non-NULL handle stored. Sprint 5 will verify the device here.
         * For now, mark as stub — actual I2C reads not implemented yet.
         *
         * Sprint 5 TODO: ping device and set EV_PROTO_STATUS_ACTIVE.
         */
        s_temp_backend = EV_PROTO_STATUS_STUB;
    }

    if (s_initialised == false)
    {
        return EV_STATUS_NOT_READY;
    }

    return EV_STATUS_OK;
}

/**
 * @brief Return current temperature sensor backend — STUB (Sprint 2–4).
 */
ev_proto_status_t sensor_get_temp_backend(void)
{
    return s_temp_backend;
}
