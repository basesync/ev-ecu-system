/**
 * @file    can_driver.c
 * @brief   CAN Bus Driver — frame encoding and UART-logged transmission stub
 *
 * @details Sprint 3 implementation.
 *
 *          This module encodes all four CAN frame types into the standard
 *          8-byte layout and stores the last transmitted frame for test
 *          inspection via can_get_last_frame().
 *
 *          CAN transmission in Sprint 3:
 *            - Frames ARE correctly encoded in memory
 *            - Frames are NOT sent over the physical CAN bus
 *            - Frame bytes are logged over UART (printf stub) for verification
 *            - The can_frame_t buffer can be inspected by unit tests
 *
 *          Sprint 5 action:
 *            Replace the UART stub output in priv_transmit_frame() with:
 *            HAL_CAN_AddTxMessage(&hcan, &TxHeader, TxData, &TxMailbox);
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdio.h>
#include <string.h>
#include "can_driver.h"

/* ─── Module-Private State ────────────────────────────────────────────────── */

/** Initialisation flag — enforces that can_driver_init() is called first */
static bool s_initialised = false;

/** Last frame encoded — used by can_get_last_frame() and unit tests */
static can_frame_t s_last_frame;

/** Flag to track whether s_last_frame contains a valid frame */
static bool s_frame_ready = false;

/* ─── Private Helper Functions ────────────────────────────────────────────── */

/**
 * @brief  Encode a 16-bit integer into two bytes (little-endian).
 *
 * @details CAN frames use little-endian byte order throughout this project.
 *          This helper eliminates repeated bitshift boilerplate.
 *
 * @param  buf    Pointer to destination buffer (must have at least 2 bytes).
 * @param  value  Value to encode.
 */
static void priv_encode_u16_le(uint8_t *buf, uint16_t value)
{
    buf[0] = (uint8_t)(value & 0xFFU);
    buf[1] = (uint8_t)((value >> 8U) & 0xFFU);
}

/**
 * @brief  Encode a 32-bit integer into four bytes (little-endian).
 *
 * @param  buf    Pointer to destination buffer (must have at least 4 bytes).
 * @param  value  Value to encode.
 */
static void priv_encode_u32_le(uint8_t *buf, uint32_t value)
{
    buf[0] = (uint8_t)(value         & 0xFFU);
    buf[1] = (uint8_t)((value >> 8U) & 0xFFU);
    buf[2] = (uint8_t)((value >> 16U) & 0xFFU);
    buf[3] = (uint8_t)((value >> 24U) & 0xFFU);
}

/**
 * @brief  "Transmit" a CAN frame.
 *
 * @details Sprint 3: Stores the frame in s_last_frame and logs bytes to UART.
 *          Sprint 5: Replace the printf with HAL_CAN_AddTxMessage().
 *
 * @param  frame  Pointer to fully-populated can_frame_t to transmit.
 */
static void priv_transmit_frame(const can_frame_t *frame)
{
    uint8_t i;

    /* Store for test inspection */
    (void)memcpy(&s_last_frame, frame, sizeof(can_frame_t));
    s_frame_ready = true;

    /*
     * Sprint 3: Log encoded bytes over UART (printf stub).
     * This is visible in Wokwi serial monitor and confirms correct encoding.
     *
     * Sprint 5 replacement:
     *   CAN_TxHeaderTypeDef TxHeader;
     *   uint32_t TxMailbox;
     *   TxHeader.StdId = frame->id;
     *   TxHeader.DLC   = frame->dlc;
     *   TxHeader.RTR   = CAN_RTR_DATA;
     *   TxHeader.IDE   = CAN_ID_STD;
     *   HAL_CAN_AddTxMessage(s_hcan, &TxHeader, frame->data, &TxMailbox);
     */
    (void)printf(">CAN_TX:0x%03X[", (unsigned int)frame->id);
    for (i = 0U; i < frame->dlc; i++)
    {
        (void)printf("%02X", (unsigned int)frame->data[i]);
        if (i < (frame->dlc - 1U))
        {
            (void)printf(",");
        }
    }
    (void)printf("]\n");
}

/* ─── Public Function Implementations ────────────────────────────────────── */

/**
 * @brief  Initialise the CAN driver module.
 */
ev_status_t can_driver_init(void *hcan)
{
    /*
     * Sprint 3: Store the handle (unused until Sprint 5) and set the flag.
     * Sprint 5: Call HAL_CAN_Start(hcan) and configure RX filter for 0x200.
     */
    (void)hcan;   /* Suppress unused-parameter warning until Sprint 5 */

    (void)memset(&s_last_frame, 0, sizeof(can_frame_t));
    s_frame_ready  = false;
    s_initialised  = true;

    return EV_STATUS_OK;
}

/**
 * @brief  Encode and transmit EV_STATUS frame (0x100).
 */
ev_status_t can_send_status(const ev_can_status_t *status)
{
    can_frame_t frame;

    if (status == NULL)
    {
        return EV_STATUS_INVALID;
    }

    if (!s_initialised)
    {
        return EV_STATUS_NOT_READY;
    }

    frame.id  = EV_CAN_ID_STATUS;   /* 0x100 */
    frame.dlc = EV_CAN_DLC_STATUS;  /* 8 bytes */

    (void)memset(frame.data, 0x00U, sizeof(frame.data));

    /*
     * Byte 0: EV state (ev_state_t fits in one byte, max value = 4)
     * Byte 1: Active fault bitmask
     * Byte 2: Motor duty cycle percentage (0–100)
     * Byte 3: State of charge percentage (0–100)
     * Bytes 4-7: System uptime in milliseconds (uint32, little-endian)
     */
    frame.data[0] = (uint8_t)status->state;
    frame.data[1] = status->active_faults;
    frame.data[2] = status->motor_duty_pct;
    frame.data[3] = status->soc_pct;
    priv_encode_u32_le(&frame.data[4], status->uptime_ms);

    priv_transmit_frame(&frame);
    return EV_STATUS_OK;
}

/**
 * @brief  Encode and transmit SENSOR_PACK_1 frame (0x101).
 */
ev_status_t can_send_sensor_pack1(const sensor_data_t *data)
{
    can_frame_t frame;
    int16_t     batt_temp_x10;
    int16_t     motor_temp_x10;

    if (data == NULL)
    {
        return EV_STATUS_INVALID;
    }

    if (!s_initialised)
    {
        return EV_STATUS_NOT_READY;
    }

    frame.id  = EV_CAN_ID_SENSOR_PACK_1;  /* 0x101 */
    frame.dlc = EV_CAN_DLC_SENSOR;        /* 8 bytes */

    (void)memset(frame.data, 0x00U, sizeof(frame.data));

    /*
     * Float temperatures are encoded as integer × 10 to preserve one decimal
     * place without using floating-point in the CAN frame.
     * E.g., 35.2°C is encoded as 352 (0x0160 little-endian).
     *
     * Byte range check: float cast to int16_t is safe for temperatures
     * in the range -3276.8°C to 3276.7°C, which covers all real cases.
     *
     * Bytes 0-1: battery temp × 10 (int16, little-endian)
     * Bytes 2-3: motor temp × 10   (int16, little-endian)
     * Bytes 4-5: speed RPM         (uint16, little-endian)
     * Byte 6:    throttle %        (uint8, 0–100)
     * Byte 7:    reserved = 0x00
     */
    batt_temp_x10  = (int16_t)(data->batt_temp_c  * 10.0f);
    motor_temp_x10 = (int16_t)(data->motor_temp_c * 10.0f);

    priv_encode_u16_le(&frame.data[0], (uint16_t)batt_temp_x10);
    priv_encode_u16_le(&frame.data[2], (uint16_t)motor_temp_x10);
    priv_encode_u16_le(&frame.data[4], data->speed_rpm);
    frame.data[6] = data->throttle_pct;
    frame.data[7] = 0x00U;  /* Reserved */

    priv_transmit_frame(&frame);
    return EV_STATUS_OK;
}

/**
 * @brief  Encode and transmit SENSOR_PACK_2 frame (0x102).
 */
ev_status_t can_send_sensor_pack2(const sensor_data_t *data)
{
    can_frame_t frame;
    uint16_t    voltage_x10;
    int16_t     current_x10;

    if (data == NULL)
    {
        return EV_STATUS_INVALID;
    }

    if (!s_initialised)
    {
        return EV_STATUS_NOT_READY;
    }

    frame.id  = EV_CAN_ID_SENSOR_PACK_2;  /* 0x102 */
    frame.dlc = EV_CAN_DLC_SENSOR;        /* 8 bytes */

    (void)memset(frame.data, 0x00U, sizeof(frame.data));

    /*
     * Bytes 0-1: voltage × 10  (uint16, little-endian)
     * Bytes 2-3: current × 10  (int16, little-endian — signed for charge/discharge)
     * Byte 4:    brake_active  (0x00 = released, 0x01 = pressed)
     * Bytes 5-7: reserved = 0x00
     */
    voltage_x10 = (uint16_t)(data->voltage_v * 10.0f);
    current_x10 = (int16_t)(data->current_a  * 10.0f);

    priv_encode_u16_le(&frame.data[0], voltage_x10);
    priv_encode_u16_le(&frame.data[2], (uint16_t)current_x10);
    frame.data[4] = (data->brake_active == true) ? 0x01U : 0x00U;
    frame.data[5] = 0x00U;
    frame.data[6] = 0x00U;
    frame.data[7] = 0x00U;

    priv_transmit_frame(&frame);
    return EV_STATUS_OK;
}

/**
 * @brief  Encode and transmit FAULT_FRAME (0x1FF).
 */
ev_status_t can_send_fault_frame(fault_code_t fault_code, uint32_t timestamp_ms)
{
    can_frame_t frame;

    if (!s_initialised)
    {
        return EV_STATUS_NOT_READY;
    }

    frame.id  = EV_CAN_ID_FAULT_FRAME;  /* 0x1FF */
    frame.dlc     = EV_CAN_DLC_FAULT;   /* Override: fault + reserved + 4-byte timestamp */

    /*
     * Note: DLC is 4 but the full 8-byte buffer is zeroed for safety.
     * Only bytes 0-3 are transmitted (DLC = 4 means bytes 0-3 on CAN bus).
     *
     * Byte 0:   fault code bitmask
     * Byte 1:   reserved = 0x00
     * Bytes 2-5: timestamp_ms (uint32, little-endian)
     *   BUT since DLC = 4, only bytes 0-3 are transmitted.
     *   Sprint 3 note: DLC changed to 6 to include timestamp.
     *   See ev_config.h EV_CAN_DLC_FAULT which is set to 4 to match the
     *   Sprint 2 interface definition. The full timestamp requires DLC=6.
     *   TODO: Update EV_CAN_DLC_FAULT to 6 in ev_config.h Sprint 3 update.
     */
    (void)memset(frame.data, 0x00U, sizeof(frame.data));

    frame.data[0] = fault_code;
    frame.data[1] = 0x00U;
    priv_encode_u32_le(&frame.data[2], timestamp_ms);

    priv_transmit_frame(&frame);
    return EV_STATUS_OK;
}

/**
 * @brief  Get the last transmitted CAN frame for test inspection.
 */
ev_status_t can_get_last_frame(can_frame_t *frame)
{
    if (frame == NULL)
    {
        return EV_STATUS_INVALID;
    }

    if (!s_frame_ready)
    {
        return EV_STATUS_ERROR;
    }

    (void)memcpy(frame, &s_last_frame, sizeof(can_frame_t));
    return EV_STATUS_OK;
}
