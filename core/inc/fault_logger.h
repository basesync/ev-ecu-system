/**
 * @file    fault_logger.h
 * @brief   Fault Logger — persistent fault storage via SPI NOR Flash
 *
 * @details This module stores fault events in W25Q32 SPI flash so they
 *          survive power cycling. On next boot, the system reads the
 *          fault log to understand recent fault history.
 *
 *          Implementation timeline:
 *            Sprint 2-5: Stub — all functions return OK, no actual storage
 *            Sprint 6:   Full implementation using SPI1 + W25Q32 driver
 *
 *          Integration point in fault_manager.c (Sprint 3+):
 *          @code
 *            if (fault_is_critical()) {
 *                (void)fault_logger_write(fault_get_active(), HAL_GetTick());
 *            }
 *          @endcode
 *
 *          At boot in main.c (Sprint 6):
 *          @code
 *            fault_logger_init(&hspi1);
 *            if (fault_logger_get_count() > 0U) {
 *                // Previous faults exist — log them to UART on boot
 *            }
 *          @endcode
 *
 * @author  BaseSync Team
 * @version 0.1.0 (Sprint 2 — Interface defined, stub implementation)
 * @date    2025
 */

#ifndef FAULT_LOGGER_H
#define FAULT_LOGGER_H

/* ─── Includes ───────────────────────────────────────────────────────────── */
#include <stdint.h>
#include <stddef.h>
#include "ev_types.h"
#include "ev_config.h"
#include "stm32f1xx_hal_spi.h"

/* ─── Public Function Declarations ──────────────────────────────────────── */

/**
 * @brief  Initialise the fault logger.
 *
 * @details Sprint 2-5 (stub): Stores the SPI handle and sets the
 *          initialised flag. Returns OK regardless of handle value.
 *
 *          Sprint 6 (real): Asserts CS high, sends READ_JEDEC_ID command,
 *          verifies manufacturer ID = EV_SPI_FLASH_JEDEC_MFR_ID (0xEF),
 *          reads flash log sector header to restore entry count.
 *
 * @param  hspi  Pointer to SPI1 handle. NULL = stub mode (no flash).
 *
 * @retval EV_STATUS_OK        Initialised successfully (or stub mode).
 * @retval EV_STATUS_INVALID   JEDEC ID mismatch — wrong chip or not wired.
 * @retval EV_STATUS_HAL_ERROR SPI communication error.
 */
ev_status_t fault_logger_init(SPI_HandleTypeDef *hspi);

/**
 * @brief  Write one fault event to the persistent log.
 *
 * @details Sprint 2-5 (stub): Does nothing, returns OK.
 *
 *          Sprint 6 (real): Assembles a fault_log_entry_t struct,
 *          computes CRC16 over fault_code + timestamp_ms, and writes
 *          the 8-byte entry to the next available flash page offset.
 *          If the log area is full, wraps to the start (circular buffer)
 *          after erasing the sector.
 *
 * @param  code         Active fault bitmask at time of event.
 * @param  timestamp_ms HAL_GetTick() value at time of event.
 *
 * @retval EV_STATUS_OK        Entry written (or no-op in stub mode).
 * @retval EV_STATUS_NOT_READY fault_logger_init() has not been called.
 * @retval EV_STATUS_HAL_ERROR SPI write failed (Sprint 6+).
 */
ev_status_t fault_logger_write(fault_code_t code, uint32_t timestamp_ms);

/**
 * @brief  Read the most recently written fault log entry.
 *
 * @details Sprint 2-5 (stub): Sets *code = FAULT_NONE, *timestamp_ms = 0,
 *          returns EV_STATUS_ERROR (empty log).
 *
 *          Sprint 6 (real): Reads the last written 8-byte entry from flash,
 *          verifies CRC16, returns the entry values.
 *
 * @param  code          Output: fault code bitmask. Must not be NULL.
 * @param  timestamp_ms  Output: timestamp in ms. Must not be NULL.
 *
 * @retval EV_STATUS_OK      Entry read and CRC verified.
 * @retval EV_STATUS_INVALID NULL output pointer passed.
 * @retval EV_STATUS_ERROR   Log is empty (consistent stub return value).
 */
ev_status_t fault_logger_read_last(fault_code_t *code, uint32_t *timestamp_ms);

/**
 * @brief  Return the number of fault entries currently in the log.
 *
 * @details Sprint 2-5 (stub): Returns 0.
 *          Sprint 6 (real): Returns entry count from flash header.
 *
 * @retval Count of stored entries (0 to EV_SPI_FLASH_LOG_MAX_ENTRIES).
 */
uint16_t fault_logger_get_count(void);

/**
 * @brief  Erase all fault log entries from flash.
 *
 * @details Sprint 2-5 (stub): Returns OK.
 *
 *          Sprint 6 (real): Issues W25Q32 WRITE_ENABLE then SECTOR_ERASE
 *          for each log sector. Polls the WIP status bit until complete.
 *          Takes up to EV_SPI_FLASH_ERASE_TIMEOUT_MS milliseconds.
 *
 *          Only call on explicit service request (e.g., CAN command 0x7FF).
 *          Do NOT call automatically — fault history is valuable.
 *
 * @retval EV_STATUS_OK        Log erased (or no-op in stub mode).
 * @retval EV_STATUS_NOT_READY fault_logger_init() not called.
 * @retval EV_STATUS_HAL_ERROR SPI erase failed (Sprint 6+).
 */
ev_status_t fault_logger_clear_all(void);

/**
 * @brief  Get the current backend status of the fault logger.
 *
 * @details Returns STUB until Sprint 6 connects real SPI flash.
 *          ACTIVE after successful fault_logger_init() with real SPI handle.
 *
 * @retval EV_PROTO_STATUS_STUB   SPI flash not connected (Sprint 2-5).
 * @retval EV_PROTO_STATUS_ACTIVE W25Q32 verified and operational (Sprint 6+).
 */
ev_proto_status_t fault_logger_get_backend_status(void);

#endif /* FAULT_LOGGER_H */
