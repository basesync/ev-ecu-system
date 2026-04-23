/**
 * @file    ev_config.h
 * @brief   All configuration constants and thresholds for the EV ECU System
 *
 * @details This is the SINGLE SOURCE OF TRUTH for all tunable values.
 *          - Fault thresholds
 *          - Timing periods
 *          - CAN IDs
 *          - Hardware pin assignments
 *          - UART configuration
 *          - PWM configuration
 *
 * @note    NO magic numbers anywhere in firmware code.
 *          If you need a number, it must be defined here.
 *          If you change a threshold, change it ONLY here.
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

#ifndef EV_CONFIG_H
#define EV_CONFIG_H

/* ===========================================================================
 * FAULT THRESHOLDS
 * All values based on a simulated 48V lithium-ion battery pack.
 * For real hardware: calibrate against datasheet specifications.
 * =========================================================================== */

/** Battery temperature thresholds (degrees Celsius) */
#define EV_BATT_TEMP_WARNING_C          (55.0f)   /* Warning: reduce power   */
#define EV_BATT_TEMP_CRITICAL_C         (60.0f)   /* Critical: enter fault   */

/** Motor temperature thresholds (degrees Celsius) */
#define EV_MOTOR_TEMP_WARNING_C         (70.0f)   /* Warning: reduce power   */
#define EV_MOTOR_TEMP_CRITICAL_C        (80.0f)   /* Critical: enter fault   */

/** Battery current thresholds (Amperes) */
#define EV_CURRENT_WARNING_A            (40.0f)   /* Warning: reduce throttle */
#define EV_CURRENT_CRITICAL_A           (50.0f)   /* Critical: enter fault   */

/** Battery voltage thresholds (Volts) — based on 13S Li-ion pack */
#define EV_VOLTAGE_MIN_WARN_V           (37.0f)   /* Under-voltage warning   */
#define EV_VOLTAGE_MIN_CRITICAL_V       (35.0f)   /* Under-voltage fault     */
#define EV_VOLTAGE_MAX_WARN_V           (54.0f)   /* Over-voltage warning    */
#define EV_VOLTAGE_MAX_CRITICAL_V       (55.0f)   /* Over-voltage fault      */

/** Battery state of charge levels (percent) */
#define EV_SOC_LOW_WARN_PCT             (20U)     /* Low battery warning     */
#define EV_SOC_CRITICAL_PCT             (10U)     /* Critical low battery    */

/* ===========================================================================
 * TIMING CONFIGURATION
 * All periods in milliseconds.
 * =========================================================================== */

/** Sensor sampling periods */
#define EV_SENSOR_TEMP_PERIOD_MS        (100U)    /* Temperature sample rate */
#define EV_SENSOR_VOLTAGE_PERIOD_MS     (50U)     /* Voltage sample rate     */
#define EV_SENSOR_CURRENT_PERIOD_MS     (50U)     /* Current sample rate     */
#define EV_SENSOR_SPEED_PERIOD_MS       (10U)     /* Speed sample rate       */
#define EV_SENSOR_THROTTLE_PERIOD_MS    (10U)     /* Throttle sample rate    */
#define EV_SENSOR_BRAKE_PERIOD_MS       (10U)     /* Brake switch read rate  */

/** CAN bus transmission periods */
#define EV_CAN_STATUS_PERIOD_MS         (100U)    /* EV_STATUS frame period  */
#define EV_CAN_SENSOR_PERIOD_MS         (100U)    /* Sensor frame period     */

/** UART logging periods */
#define EV_LOGGER_PERIOD_MS             (100U)    /* Serial log period       */

/** Watchdog timer configuration */
#define EV_WATCHDOG_TIMEOUT_MS          (500U)    /* WDG resets if not fed   */
#define EV_WATCHDOG_FEED_PERIOD_MS      (100U)    /* Feed every 100ms        */

/** Motor soft-start ramp time */
#define EV_MOTOR_SOFTSTART_MS           (500U)    /* 0 to target in 500ms   */

/** GPIO debounce time */
#define EV_GPIO_DEBOUNCE_MS             (20U)     /* Switch debounce time    */

/* ===========================================================================
 * CAN BUS CONFIGURATION
 * CAN IDs use 11-bit standard identifiers (0x000 to 0x7FF)
 * =========================================================================== */

/** CAN bus baud rate */
#define EV_CAN_BAUD_RATE_KBPS           (500U)    /* 500 kbps standard      */

/** CAN Message IDs */
#define EV_CAN_ID_STATUS                (0x100U)  /* EV status frame        */
#define EV_CAN_ID_SENSOR_PACK_1         (0x101U)  /* Temp + speed frame     */
#define EV_CAN_ID_SENSOR_PACK_2         (0x102U)  /* Voltage + current      */
#define EV_CAN_ID_FAULT_FRAME           (0x1FFU)  /* Fault notification     */

/** CAN frame data lengths (bytes) */
#define EV_CAN_DLC_STATUS               (8U)
#define EV_CAN_DLC_SENSOR               (8U)
#define EV_CAN_DLC_FAULT                (4U)

/* ===========================================================================
 * UART / LOGGER CONFIGURATION
 * =========================================================================== */

#define EV_UART_BAUD_RATE               (115200U) /* UART baud rate          */
#define EV_UART_TX_BUFFER_SIZE          (256U)    /* TX buffer size in bytes */

/* ===========================================================================
 * MOTOR / PWM CONFIGURATION
 * =========================================================================== */

#define EV_MOTOR_PWM_FREQUENCY_HZ       (20000U)  /* 20kHz PWM frequency     */
#define EV_MOTOR_MIN_DUTY_PCT           (0U)      /* 0% = fully stopped      */
#define EV_MOTOR_MAX_DUTY_PCT           (100U)    /* 100% = full speed       */
#define EV_THROTTLE_DEADBAND_PCT        (5U)      /* Below 5% = no movement  */

/* ===========================================================================
 * ADC CONFIGURATION
 * =========================================================================== */

#define EV_ADC_RESOLUTION_BITS          (12U)     /* 12-bit ADC              */
#define EV_ADC_MAX_VALUE                (4095U)   /* 2^12 - 1                */
#define EV_ADC_VREF_MV                  (3300U)   /* 3.3V reference in mV    */

/** LM35 temperature sensor: 10mV per degree Celsius */
#define EV_TEMP_SENSOR_MV_PER_DEG       (10.0f)

/** ACS712 current sensor: 185mV/A for 5A model, 100mV/A for 20A model */
#define EV_CURRENT_SENSOR_MV_PER_AMP   (100.0f)  /* Using ACS712-20A        */
#define EV_CURRENT_SENSOR_MIDPOINT_MV  (2500U)   /* 0A output at 2.5V       */

/** Voltage divider ratio for battery voltage sensing */
/** Using R1=100kΩ, R2=10kΩ -> ratio = 10/(100+10) = 0.0909 */
#define EV_VOLTAGE_DIVIDER_RATIO        (0.0909f)

/* ===========================================================================
 * STM32 PIN ASSIGNMENTS
 * Logical names for hardware pins. Change here if hardware changes.
 * =========================================================================== */

/** ADC Channel assignments */
#define EV_PIN_ADC_BATT_TEMP            (0U)      /* PA0 -> ADC1_CH0         */
#define EV_PIN_ADC_MOTOR_TEMP           (1U)      /* PA1 -> ADC1_CH1         */
#define EV_PIN_ADC_CURRENT              (2U)      /* PA2 -> ADC1_CH2         */
#define EV_PIN_ADC_VOLTAGE              (3U)      /* PA3 -> ADC1_CH3         */
#define EV_PIN_ADC_THROTTLE             (4U)      /* PA4 -> ADC1_CH4         */

/** GPIO assignments (logical names only — actual GPIO in BSP) */
#define EV_NUM_ADC_CHANNELS             (5U)      /* Total ADC channels used */

/* ===========================================================================
 * SYSTEM CONFIGURATION
 * =========================================================================== */

#define EV_FIRMWARE_VERSION_MAJOR       (0U)
#define EV_FIRMWARE_VERSION_MINOR       (1U)
#define EV_FIRMWARE_VERSION_PATCH       (0U)

/** Magic number written to backup RAM to request DFU mode (bootloader use) */
#define EV_DFU_REQUEST_MAGIC            (0xDEADBEEFU)

/** Fault log size in Flash (number of fault entries to store) */
#define EV_FAULT_LOG_MAX_ENTRIES        (32U)

#endif /* EV_CONFIG_H */
