/**
 * @file    ev_types.h
 * @brief   Shared type definitions for the EV ECU firmware
 *
 * @details This header defines all shared types used across modules:
 *          - Status codes (ev_status_t)
 *          - Vehicle state machine states (ev_state_t)
 *          - Fault codes (bitmask defines)
 *          - Sensor data structure (sensor_data_t)
 *
 * @note    This file is included by ALL modules.
 *          Keep it lean - only types, enums, and structs here.
 *          Constants and thresholds go in ev_config.h.
 *          Do NOT include any STM32 HAL headers here.
 *
 * @author  BaseSync Team
 * @version 1.0
 * @date    2025
 */

#ifndef EV_TYPES_H
#define EV_TYPES_H

/* --- Standard Library Includes ----------------------------------------------
 * These are the ONLY headers allowed in ev_types.h
 */
#include <stdint.h>     /* uint8_t, uint16_t, uint32_t, int16_t, etc. */
#include <stdbool.h>    /* bool, true, false */

/* ===========================================================================
 * STATUS CODES
 * Returned by all functions that can fail.
 * =========================================================================== */

/**
 * @brief  Return status codes for all EV ECU functions.
 *
 * Every function that can fail must return ev_status_t.
 * Callers must check the return value - never ignore it.
 *
 * Example:
 *   if (sensor_init() != EV_STATUS_OK) {
 *       fault_set(FAULT_SENSOR_INIT_FAILED);
 *   }
 */
typedef enum
{
    EV_STATUS_OK         = 0U,  /* Operation completed successfully */
    EV_STATUS_ERROR      = 1U,  /* Generic error */
    EV_STATUS_TIMEOUT    = 2U,  /* Operation timed out */
    EV_STATUS_INVALID    = 3U,  /* Invalid parameter (e.g., NULL pointer) */
    EV_STATUS_HAL_ERROR  = 4U,  /* STM32 HAL returned an error */
    EV_STATUS_NOT_READY  = 5U   /* Hardware not yet initialised */
} ev_status_t;

/* ===========================================================================
 * EV STATE MACHINE STATES
 * The ECU operates as a state machine. These are the possible states.
 * =========================================================================== */

/**
 * @brief  EV ECU operating state machine states.
 *
 * State transitions:
 *   INIT -> IDLE        : All peripherals initialised successfully
 *   IDLE -> RUNNING     : Throttle > EV_THROTTLE_MIN_PCT, no faults
 *   RUNNING -> IDLE     : Throttle = 0 AND brake active
 *   RUNNING -> FAULT    : Any critical fault detected
 *   IDLE -> FAULT       : Any critical fault detected
 *   FAULT -> SAFE_STATE : Automatic (immediate on fault detection)
 *   SAFE_STATE -> IDLE  : Manual reset command received
 */
typedef enum
{
    EV_STATE_INIT       = 0U,   /* Power-on: initialising hardware */
    EV_STATE_IDLE       = 1U,   /* Sensors running, motor stopped */
    EV_STATE_RUNNING    = 2U,   /* Motor active, all systems nominal */
    EV_STATE_FAULT      = 3U,   /* Fault detected, transitioning to safe */
    EV_STATE_SAFE_STATE = 4U    /* All outputs off, waiting for reset */
} ev_state_t;

/* ===========================================================================
 * FAULT CODES
 * Stored as a bitmask. Multiple faults can be active simultaneously.
 * Use bitwise OR to set: faults |= FAULT_OVER_TEMP_BATT
 * Use bitwise AND to check: if (faults & FAULT_OVER_TEMP_BATT)
 * =========================================================================== */

/** @defgroup FaultCodes Fault Code Bitmask Definitions
 * Each fault occupies one bit. Up to 8 faults on a uint8_t.
 * @{
 */
#define FAULT_NONE              (0x00U)  /* No active faults */
#define FAULT_OVER_TEMP_BATT    (0x01U)  /* Battery temperature > threshold */
#define FAULT_OVER_TEMP_MOTOR   (0x02U)  /* Motor temperature > threshold */
#define FAULT_OVER_CURRENT      (0x04U)  /* Battery current > threshold */
#define FAULT_UNDER_VOLTAGE     (0x08U)  /* Battery voltage < minimum */
#define FAULT_OVER_VOLTAGE      (0x10U)  /* Battery voltage > maximum */
#define FAULT_WATCHDOG          (0x20U)  /* Watchdog timer expired */
#define FAULT_CAN_ERROR         (0x40U)  /* CAN bus communication error */
#define FAULT_MANUAL_TRIGGER    (0x80U)  /* Manual fault switch activated */
#define FAULT_INVALID_DATA      (0xFFU)  /* Invalid input to fault check */
/** @} */

/** Convenience type for fault code storage */
typedef uint8_t fault_code_t;

/* ===========================================================================
 * SENSOR DATA STRUCTURE
 * Holds one complete snapshot of all sensor readings.
 * Populated by sensor_read_all() and passed to other modules.
 * =========================================================================== */

/**
 * @brief  Complete sensor data snapshot.
 *
 * All sensor values at a single point in time.
 * Always use sensor_read_all() to populate this - never set fields manually
 * in production code (only in unit tests via mocks).
 */
typedef struct
{
    float    batt_temp_c;     /* Battery temperature in degrees Celsius     */
    float    motor_temp_c;    /* Motor temperature in degrees Celsius        */
    float    current_a;       /* Battery pack current in Amperes (+ = discharge) */
    float    voltage_v;       /* Battery pack voltage in Volts               */
    uint16_t speed_rpm;       /* Motor/wheel speed in RPM                    */
    uint8_t  throttle_pct;    /* Throttle position: 0 = released, 100 = full */
    bool     brake_active;    /* true = brake is pressed                     */
    bool     fault_switch;    /* true = manual fault trigger switch pressed  */
} sensor_data_t;

/* ===========================================================================
 * EV STATUS STRUCTURE
 * Transmitted periodically over CAN bus (CAN ID 0x100)
 * =========================================================================== */

/**
 * @brief  EV system status snapshot for CAN transmission.
 */
typedef struct
{
    ev_state_t   state;           /* Current EV state machine state        */
    fault_code_t active_faults;   /* Bitmask of all currently active faults */
    uint8_t      motor_duty_pct;  /* Current motor PWM duty cycle (0–100%) */
    uint8_t      soc_pct;         /* State of Charge estimate (0–100%)     */
    uint32_t     uptime_ms;       /* System uptime in milliseconds          */
} ev_can_status_t;

/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 6 — FAULT LOG ENTRY  (Sprint 6 — SPI Flash storage)
 *
 * Each fault event written to W25Q32 SPI flash uses this structure.
 * Size: exactly 8 bytes — packs into one SPI flash page efficiently.
 * CRC16 covers fault_code and timestamp_ms for integrity checking.
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief One fault log entry written to SPI NOR Flash (Sprint 6).
 *
 * @details Layout (8 bytes total):
 *   Byte 0:   fault_code    — bitmask of all faults active at event time
 *   Byte 1:   reserved      — set to 0x00 for future use
 *   Bytes 2-3: crc16        — CRC-16/CCITT over bytes 0 and 4-7
 *   Bytes 4-7: timestamp_ms — HAL_GetTick() value at time of fault event
 */
typedef struct
{
    fault_code_t fault_code;    /**< Active fault bitmask at event time   */
    uint8_t      reserved;      /**< Alignment padding — always 0x00      */
    uint16_t     crc16;         /**< CRC-16 integrity check               */
    uint32_t     timestamp_ms;  /**< System uptime in ms when fault fired */
} fault_log_entry_t;

/* ═══════════════════════════════════════════════════════════════════════════
 * SECTION 7 — PROTOCOL STATUS ENUM  (Sprint 5/6 — UART, I2C, SPI)
 *
 * Tracks whether each hardware protocol is running on real hardware
 * or using a stub. Allows graceful degradation and diagnostic logging.
 * ═══════════════════════════════════════════════════════════════════════════ */

/**
 * @brief Status of a communication protocol peripheral.
 *
 * @details Used by sensor_get_temp_backend() and logger_get_uart_status()
 *          so that diagnostic messages can report hardware availability.
 *
 * Example usage:
 * @code
 *   if (sensor_get_temp_backend() == EV_PROTO_STATUS_STUB) {
 *       // Still using ADC simulation — log a warning
 *   }
 * @endcode
 */
typedef enum
{
    EV_PROTO_STATUS_NOT_USED = 0U,  /**< Protocol not connected in this HW rev */
    EV_PROTO_STATUS_STUB     = 1U,  /**< Using simulation stub (Sprint 1-4)    */
    EV_PROTO_STATUS_ACTIVE   = 2U   /**< Real HAL configured and running        */
} ev_proto_status_t;

#endif /* EV_TYPES_H */
