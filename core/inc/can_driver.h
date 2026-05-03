/**
 * @file    can_driver.h
 * @brief   CAN Bus Driver — frame encoding and transmission
 *
 * @details This module encodes EV system data into CAN frames and
 *          transmits them via the STM32 CAN1 peripheral through the
 *          TJA1050 transceiver on PB8 (TX) and PB9 (RX).
 *
 *          CAN Frame Layout (500 kbps, standard 11-bit IDs):
 *
 *          0x100 — EV_STATUS (every 100ms):
 *            Byte 0:   current state (ev_state_t)
 *            Byte 1:   active fault bitmask (fault_code_t)
 *            Byte 2:   motor duty % (0–100)
 *            Byte 3:   SOC % (0–100)
 *            Bytes 4-7: uptime_ms (uint32_t, little-endian)
 *
 *          0x101 — SENSOR_PACK_1 (every 100ms):
 *            Bytes 0-1: battery temp × 10 (int16, little-endian)
 *            Bytes 2-3: motor temp × 10 (int16, little-endian)
 *            Bytes 4-5: speed RPM (uint16, little-endian)
 *            Byte 6:    throttle % (uint8)
 *            Byte 7:    reserved = 0x00
 *
 *          0x102 — SENSOR_PACK_2 (every 100ms):
 *            Bytes 0-1: voltage × 10 (uint16, little-endian)
 *            Bytes 2-3: current × 10 (int16, little-endian)
 *            Byte 4:    brake_active (0x00 or 0x01)
 *            Bytes 5-7: reserved = 0x00
 *
 *          0x1FF — FAULT_FRAME (on-event only, not periodic):
 *            Byte 0:   fault code bitmask
 *            Byte 1:   reserved = 0x00
 *            Bytes 2-5: timestamp_ms (uint32_t, little-endian)
 *            Bytes 6-7: reserved = 0x00
 *
 *          Sprint 3: CAN1 peripheral is a stub — frames are encoded in
 *          memory and logged over UART for verification. Real CAN hardware
 *          transmission is enabled in Sprint 5 hardware bring-up.
 *
 *          Sprint 5 action: Replace CAN stub HAL with real MX_CAN_Init()
 *          and HAL_CAN_AddTxMessage() calls from CubeMX.
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

#ifndef CAN_DRIVER_H
#define CAN_DRIVER_H

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdint.h>
#include <stddef.h>
#include "ev_types.h"
#include "ev_config.h"

/* ─── Public Type Definitions ─────────────────────────────────────────────── */

/**
 * @brief  Raw CAN frame data container.
 *
 * @details Mirrors the real HAL_CAN TxHeader + data layout so that
 *          test code can inspect exactly what was encoded without
 *          needing a real CAN peripheral.
 */
typedef struct
{
    uint32_t id;            /**< 11-bit standard CAN identifier     */
    uint8_t  dlc;           /**< Data length code (0–8 bytes)       */
    uint8_t  data[8];       /**< Frame payload bytes                 */
} can_frame_t;

/* ─── Public Function Declarations ───────────────────────────────────────── */

/**
 * @brief  Initialise the CAN driver module.
 *
 * @details Sprint 3 (stub): Stores the CAN handle and sets the
 *          initialised flag. No real hardware access.
 *
 *          Sprint 5 (real): Calls HAL_CAN_Start() to enable the
 *          CAN peripheral after MX_CAN_Init() has configured it.
 *          Also configures the RX filter to accept 0x200 (speed command).
 *
 * @param  hcan  Pointer to CAN handle. NULL = log-only stub mode.
 *
 * @retval EV_STATUS_OK      Initialised (stub mode if hcan == NULL).
 * @retval EV_STATUS_INVALID hcan non-NULL but HAL_CAN_Start() failed.
 */
ev_status_t can_driver_init(void *hcan);

/**
 * @brief  Encode and transmit the EV_STATUS frame (CAN ID 0x100).
 *
 * @details Called every 100ms from the main loop.
 *          Encodes the current EV state, fault code, motor duty, SOC,
 *          and uptime into the 8-byte CAN frame format.
 *
 *          Sprint 3: Encodes frame to internal can_frame_t buffer
 *          and prints over UART. No actual CAN hardware transmission.
 *
 * @param  status  Pointer to current ev_status_t snapshot. Must not be NULL.
 *
 * @retval EV_STATUS_OK       Frame encoded and transmitted (or logged).
 * @retval EV_STATUS_INVALID  status pointer is NULL.
 * @retval EV_STATUS_NOT_READY can_driver_init() not called.
 */
ev_status_t can_send_status(const ev_can_status_t *status);

/**
 * @brief  Encode and transmit SENSOR_PACK_1 frame (CAN ID 0x101).
 *
 * @details Packs temperature, speed, and throttle sensor data.
 *          Called every 100ms from the main loop.
 *
 * @param  data  Pointer to sensor data snapshot. Must not be NULL.
 *
 * @retval EV_STATUS_OK, EV_STATUS_INVALID, EV_STATUS_NOT_READY.
 */
ev_status_t can_send_sensor_pack1(const sensor_data_t *data);

/**
 * @brief  Encode and transmit SENSOR_PACK_2 frame (CAN ID 0x102).
 *
 * @details Packs voltage, current, and brake status data.
 *          Called every 100ms from the main loop.
 *
 * @param  data  Pointer to sensor data snapshot. Must not be NULL.
 *
 * @retval EV_STATUS_OK, EV_STATUS_INVALID, EV_STATUS_NOT_READY.
 */
ev_status_t can_send_sensor_pack2(const sensor_data_t *data);

/**
 * @brief  Encode and transmit a FAULT_FRAME (CAN ID 0x1FF).
 *
 * @details Called immediately when a new fault is detected.
 *          This is an on-event frame — it is NOT sent periodically.
 *          The fault_manager calls this via the state machine transition.
 *
 *          In BusMaster (HIL testing), this frame is used to confirm
 *          that the ECU detected and reported the correct fault code
 *          within the required response time.
 *
 * @param  fault_code    Active fault bitmask to report.
 * @param  timestamp_ms  HAL_GetTick() value at time of fault detection.
 *
 * @retval EV_STATUS_OK, EV_STATUS_NOT_READY.
 */
ev_status_t can_send_fault_frame(fault_code_t fault_code, uint32_t timestamp_ms);

/**
 * @brief  Get the last frame that was transmitted for test inspection.
 *
 * @details Used by unit tests to verify that the correct bytes were
 *          encoded in the CAN frame without needing real CAN hardware.
 *
 *          Also used in the main loop UART log to print CAN frame bytes
 *          in Sprint 3 before real CAN hardware is connected.
 *
 * @param  frame  Pointer to can_frame_t to populate. Must not be NULL.
 *
 * @retval EV_STATUS_OK      Frame copied successfully.
 * @retval EV_STATUS_INVALID frame pointer is NULL.
 * @retval EV_STATUS_ERROR   No frame has been transmitted yet.
 */
ev_status_t can_get_last_frame(can_frame_t *frame);

#endif /* CAN_DRIVER_H */
