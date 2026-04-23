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
<<<<<<< HEAD
=======
 * UART / LOGGER CONFIGURATION
 * =========================================================================== */

#define EV_UART_BAUD_RATE               (115200U) /* UART baud rate          */
#define EV_UART_TX_BUFFER_SIZE          (256U)    /* TX buffer size in bytes */

/* ===========================================================================
>>>>>>> main
 * MOTOR / PWM CONFIGURATION
 * =========================================================================== */

#define EV_MOTOR_PWM_FREQUENCY_HZ       (20000U)  /* 20kHz PWM frequency     */
#define EV_MOTOR_MIN_DUTY_PCT           (0U)      /* 0% = fully stopped      */
#define EV_MOTOR_MAX_DUTY_PCT           (100U)    /* 100% = full speed       */
<<<<<<< HEAD
/**
 * Throttle deadband: throttle values below this percentage are treated as
 * zero. Prevents motor creep when the throttle pot is not perfectly at 0.
 */
#define EV_THROTTLE_DEADBAND_PCT        (5U)
=======
#define EV_THROTTLE_DEADBAND_PCT        (5U)      /* Below 5% = no movement  */
>>>>>>> main

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

<<<<<<< HEAD
/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 7 — ENCODER CONFIGURATION  (Sprint 2 addition)
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * Encoder pulses per revolution.
 * For a typical 600 PPR quadrature encoder in TIM encoder mode (x4):
 * effective PPR = 600 * 4 = 2400.
 *
 * Change this value to match your actual encoder specification.
 * Sprint 5: verify against datasheet of the encoder used on hardware.
 */
#define EV_ENCODER_PPR                  (2400U)

=======
>>>>>>> main
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
<<<<<<< HEAD
#define EV_FIRMWARE_VERSION_MINOR       (2U)
=======
#define EV_FIRMWARE_VERSION_MINOR       (1U)
>>>>>>> main
#define EV_FIRMWARE_VERSION_PATCH       (0U)

/** Magic number written to backup RAM to request DFU mode (bootloader use) */
#define EV_DFU_REQUEST_MAGIC            (0xDEADBEEFU)

/** Fault log size in Flash (number of fault entries to store) */
#define EV_FAULT_LOG_MAX_ENTRIES        (32U)

<<<<<<< HEAD
/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 9 — UART CONFIGURATION
 * Active: Sprint 5 (replaces printf() stubs with HAL_UART_Transmit())
 *
 * Peripheral : USART1
 * TX pin     : PA9
 * RX pin     : PA10
 * Format     : Teleplot  ">label:value\n"
 * ═══════════════════════════════════════════════════════════════════════════ */

/** Baud rate — must match Teleplot and serial terminal settings */
#define EV_UART_BAUD_RATE               (115200U)

/** Maximum characters in one Teleplot log line */
#define EV_UART_LOG_LINE_MAX_LEN        (64U)

/** HAL transmit timeout in milliseconds */
#define EV_UART_TX_TIMEOUT_MS           (10U)

/** Number of log lines in the transmit ring buffer (Sprint 5 DMA mode) */
#define EV_UART_TX_BUFFER_LINES         (8U)

/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 10 — I2C CONFIGURATION
 * Active: Sprint 5 (TMP102 replaces ADC pot for battery temperature)
 *
 * Peripheral : I2C1
 * SCL pin    : PB6   (requires 4.7 kΩ pull-up to 3.3 V)
 * SDA pin    : PB7   (requires 4.7 kΩ pull-up to 3.3 V)
 * Device     : TMP102 digital temperature sensor
 * Datasheet  : https://www.ti.com/lit/ds/symlink/tmp102.pdf
 * ═══════════════════════════════════════════════════════════════════════════ */

/** I2C bus speed — 400 kHz fast mode */
#define EV_I2C_SPEED_HZ                 (400000U)

/** TMP102 default 7-bit address when ADD0 pin is tied to GND */
#define EV_I2C_TMP102_ADDR_7BIT         (0x48U)

/**
 * STM32 HAL functions expect the 7-bit address shifted left by one.
 * HAL_I2C_Master_Receive(hi2c, EV_I2C_TMP102_ADDR_8BIT, ...)
 */
#define EV_I2C_TMP102_ADDR_8BIT         (EV_I2C_TMP102_ADDR_7BIT << 1U)

/** TMP102 internal register addresses */
#define EV_I2C_TMP102_REG_TEMP          (0x00U)  /**< Temperature result (read)  */
#define EV_I2C_TMP102_REG_CONFIG        (0x01U)  /**< Configuration (r/w)        */
#define EV_I2C_TMP102_REG_TLOW         (0x02U)  /**< Low alert threshold (r/w)  */
#define EV_I2C_TMP102_REG_THIGH        (0x03U)  /**< High alert threshold (r/w) */

/** TMP102 temperature resolution: 12-bit, 0.0625 °C per bit */
#define EV_I2C_TMP102_DEG_PER_LSB      (0.0625f)

/** Number of bytes to read from TMP102 temperature register */
#define EV_I2C_TMP102_BYTES_PER_READ    (2U)

/** HAL_I2C timeout for each transaction */
#define EV_I2C_TIMEOUT_MS               (10U)

/** Retry limit when TMP102 returns NAK */
#define EV_I2C_MAX_RETRIES              (3U)

/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 11 — SPI FLASH CONFIGURATION
 * Active: Sprint 6 (W25Q32 stores fault log in non-volatile flash)
 *
 * Peripheral : SPI1
 * MOSI pin   : PA7
 * MISO pin   : PA6
 * SCK  pin   : PA5
 * CS   pin   : PA4  (GPIO output, software-controlled, active-low)
 * Device     : W25Q32 4 MB NOR Flash (Winbond)
 * Datasheet  : https://www.winbond.com/resource-files/w25q32jv%20revg%2003272018%20plus.pdf
 * ═══════════════════════════════════════════════════════════════════════════ */

/** SPI clock — 18 MHz is well within W25Q32's 80 MHz maximum */
#define EV_SPI_FLASH_SPEED_HZ           (18000000U)

/** W25Q32 JEDEC manufacturer ID — read with CMD 0x9F to verify chip */
#define EV_SPI_FLASH_JEDEC_MFR_ID       (0xEFU)

/** W25Q32 JEDEC device ID (16-bit) */
#define EV_SPI_FLASH_JEDEC_DEV_ID       (0x4016U)

/** Flash geometry */
#define EV_SPI_FLASH_PAGE_SIZE_BYTES    (256U)     /**< Minimum write unit     */
#define EV_SPI_FLASH_SECTOR_SIZE_BYTES  (4096U)    /**< Minimum erase unit     */

/**
 * Fault log occupies first 2 sectors of flash (8 KB total).
 * This keeps fault data safely away from any future firmware area.
 */
#define EV_SPI_FLASH_LOG_BASE_ADDR      (0x000000UL)
#define EV_SPI_FLASH_LOG_SIZE_BYTES     (EV_SPI_FLASH_SECTOR_SIZE_BYTES * 2U)

/**
 * Each fault_log_entry_t is 8 bytes.
 * Maximum entries = 8192 / 8 = 1024.
 */
#define EV_SPI_FLASH_LOG_ENTRY_SIZE     (8U)
#define EV_SPI_FLASH_LOG_MAX_ENTRIES    (EV_SPI_FLASH_LOG_SIZE_BYTES \
                                         / EV_SPI_FLASH_LOG_ENTRY_SIZE)

/** CS (chip select) GPIO port and pin */
#define EV_SPI_FLASH_CS_PORT            GPIOA
#define EV_SPI_FLASH_CS_PIN             GPIO_PIN_4

/** Operation timeouts */
#define EV_SPI_FLASH_TX_TIMEOUT_MS      (100U)
#define EV_SPI_FLASH_RX_TIMEOUT_MS      (100U)
#define EV_SPI_FLASH_ERASE_TIMEOUT_MS   (400U)   /**< Sector erase: 45ms typ, 400ms max */
#define EV_SPI_FLASH_WRITE_TIMEOUT_MS   (3U)     /**< Page program: 0.7ms typ, 3ms max  */

/** W25Q32 SPI command bytes */
#define EV_SPI_FLASH_CMD_WRITE_ENABLE   (0x06U)  /**< Must precede any write/erase     */
#define EV_SPI_FLASH_CMD_WRITE_DISABLE  (0x04U)  /**< Disable write after operation    */
#define EV_SPI_FLASH_CMD_READ_STATUS1   (0x05U)  /**< Read status register 1           */
#define EV_SPI_FLASH_CMD_PAGE_PROGRAM   (0x02U)  /**< Write up to 256 bytes            */
#define EV_SPI_FLASH_CMD_READ_DATA      (0x03U)  /**< Read bytes at 24-bit address     */
#define EV_SPI_FLASH_CMD_SECTOR_ERASE   (0x20U)  /**< Erase one 4 KB sector            */
#define EV_SPI_FLASH_CMD_CHIP_ERASE     (0xC7U)  /**< Erase entire chip (use carefully)*/
#define EV_SPI_FLASH_CMD_READ_JEDEC_ID  (0x9FU)  /**< Read manufacturer + device ID    */
#define EV_SPI_FLASH_CMD_POWER_DOWN     (0xB9U)  /**< Enter deep power-down mode       */
#define EV_SPI_FLASH_CMD_RELEASE_PD     (0xABU)  /**< Exit deep power-down mode        */

/** Status register bit mask: WIP = Write In Progress */
#define EV_SPI_FLASH_STATUS_WIP_MASK    (0x01U)

=======
>>>>>>> main
#endif /* EV_CONFIG_H */
