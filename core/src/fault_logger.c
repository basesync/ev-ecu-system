/**
 * @file    fault_logger.c
 * @brief   Fault Logger — STUB implementation (Sprint 2–5)
 *
 * @details All functions here are stubs that compile cleanly, pass all
 *          CI checks, and allow other modules (fault_manager, main) to
 *          call fault_logger functions without errors from Sprint 3 onward.
 *
 *          Sprint 6 action: Delete this file and replace with the real
 *          implementation using HAL_SPI_Transmit/Receive + W25Q32 commands.
 *
 *          What the real Sprint 6 implementation will add:
 *          - W25Q32 JEDEC ID verification on init
 *          - Page Program command for fault_logger_write()
 *          - Read Data command for fault_logger_read_last()
 *          - Sector Erase command for fault_logger_clear_all()
 *          - CRC16 calculation for entry integrity
 *          - Circular buffer management across flash sectors
 *
 * @author  BaseSync Team
 * @version 0.1.0 (Sprint 2 stub)
 * @date    2025
 */

/* ─── Includes ───────────────────────────────────────────────────────────── */
#include "fault_logger.h"

/* ─── Private variables ──────────────────────────────────────────────────── */

/** SPI handle stored at init. NULL means stub mode (no real flash). */
static SPI_HandleTypeDef *s_hspi            = NULL;

/** Set true after fault_logger_init() succeeds. */
static bool               s_initialised     = false;

/** Backend status — STUB until Sprint 6 wires real SPI flash. */
static ev_proto_status_t  s_backend_status  = EV_PROTO_STATUS_STUB;

/* ─── Public Function Implementations (Stubs) ────────────────────────────── */

ev_status_t fault_logger_init(SPI_HandleTypeDef *hspi)
{
    /*
     * Store the handle. NULL = stub mode, valid handle = ready for Sprint 6.
     *
     * Sprint 6 TODO: After storing handle, send READ_JEDEC_ID (0x9F),
     * read 3 bytes, verify byte[0] == EV_SPI_FLASH_JEDEC_MFR_ID,
     * read flash log header to restore entry count from previous session.
     */
    s_hspi        = hspi;
    s_initialised = true;

    /*
     * Mark as STUB. Sprint 6 changes this to EV_PROTO_STATUS_ACTIVE
     * only after JEDEC ID is verified.
     */
    s_backend_status = EV_PROTO_STATUS_STUB;

    return EV_STATUS_OK;
}

ev_status_t fault_logger_write(fault_code_t code, uint32_t timestamp_ms)
{
    /* Suppress unused parameter warnings — parameters used in Sprint 6 */
    (void)code;
    (void)timestamp_ms;

    if (s_initialised == false)
    {
        return EV_STATUS_NOT_READY;
    }

    /*
     * STUB: Do nothing. Sprint 6 TODO:
     *   1. Assemble fault_log_entry_t:
     *        entry.fault_code   = code
     *        entry.reserved     = 0x00
     *        entry.timestamp_ms = timestamp_ms
     *        entry.crc16        = crc16_ccitt(entry bytes 0, 4-7)
     *   2. Calculate target flash address (base + entry_count * 8)
     *   3. If address >= log area end: erase next sector, wrap pointer
     *   4. Assert CS LOW
     *   5. Send WRITE_ENABLE (0x06), deassert CS
     *   6. Assert CS LOW
     *   7. Send PAGE_PROGRAM (0x02) + 24-bit address + 8 data bytes
     *   8. Deassert CS HIGH
     *   9. Poll STATUS_REG WIP bit until clear (< 3ms)
     *  10. Increment entry count in RAM
     */

    return EV_STATUS_OK;
}

ev_status_t fault_logger_read_last(fault_code_t *code, uint32_t *timestamp_ms)
{
    if ((code == NULL) || (timestamp_ms == NULL))
    {
        return EV_STATUS_INVALID;
    }

    /*
     * STUB: Return safe empty-log defaults.
     * EV_STATUS_ERROR signals "no entries" — consistent with Sprint 6 behaviour
     * when the log has zero entries in it.
     *
     * Sprint 6 TODO:
     *   1. Calculate address of last entry
     *   2. Assert CS LOW
     *   3. Send READ_DATA (0x03) + 24-bit address
     *   4. Read 8 bytes into fault_log_entry_t
     *   5. Deassert CS HIGH
     *   6. Verify CRC16 of received data
     *   7. Populate *code and *timestamp_ms from verified entry
     */
    *code         = FAULT_NONE;
    *timestamp_ms = 0U;

    return EV_STATUS_ERROR;  /* Empty log — correct stub behaviour */
}

uint16_t fault_logger_get_count(void)
{
    /*
     * STUB: Always 0. Sprint 6 reads this from a flash header sector
     * on init and increments on each write.
     */
    return 0U;
}

ev_status_t fault_logger_clear_all(void)
{
    if (s_initialised == false)
    {
        return EV_STATUS_NOT_READY;
    }

    /*
     * STUB: Do nothing. Sprint 6 TODO:
     *   For each log sector (2 sectors = 8 KB):
     *     1. Assert CS LOW
     *     2. Send WRITE_ENABLE (0x06), deassert CS
     *     3. Assert CS LOW
     *     4. Send SECTOR_ERASE (0x20) + 24-bit sector address
     *     5. Deassert CS HIGH
     *     6. Poll WIP bit until clear (up to 400ms per sector)
     *   Reset entry count in RAM to 0.
     */

    return EV_STATUS_OK;
}

ev_proto_status_t fault_logger_get_backend_status(void)
{
    return s_backend_status;
}
