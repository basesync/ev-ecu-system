/**
 * @file    mock_stm32_hal_spi.h
 * @brief   Mock STM32 SPI HAL for unit testing
 *
 * @details Allows fault_logger.c stub to compile and run in the
 *          test build without real SPI hardware.
 *          Real SPI mock with full command simulation added in Sprint 6.
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 2 — minimal stub for compilation)
 */

#ifndef MOCK_STM32_HAL_SPI_H
#define MOCK_STM32_HAL_SPI_H

#include "stm32f1xx_hal_spi.h"
#include <stdbool.h>

/** Reset all SPI mock state. Call in setUp(). */
void mock_spi_reset(void);

/** Force HAL_SPI_Transmit to return HAL_ERROR. */
void mock_spi_set_transmit_error(bool force_error);

/** Force HAL_SPI_Receive to return HAL_ERROR. */
void mock_spi_set_receive_error(bool force_error);

/** Set bytes that HAL_SPI_TransmitReceive will write into pRxData. */
void mock_spi_set_rx_data(const uint8_t *data, uint16_t len);

/** Get pointer to shared mock SPI instance for handle setup. */
uint32_t mock_spi_get_instance(void);

#endif /* MOCK_STM32_HAL_SPI_H */
