/**
 * @file    mock_stm32_hal_spi.c
 * @brief   Mock STM32 SPI HAL implementation for unit testing
 *
 * @details Sprint 2: Minimal mock so fault_logger.c stub compiles
 *          and runs in the test build without real SPI hardware.
 *          Sprint 6: Expand with W25Q32 command-level simulation.
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 2 — minimal stub for compilation)
 */

#include "mock_stm32_hal_spi.h"
#include <string.h>
#include <stdbool.h>

/* ─── Mock state ─────────────────────────────────────────────────────────── */
static bool     s_transmit_error  = false;
static bool     s_receive_error   = false;
static uint8_t  s_rx_data[256];
static uint16_t s_rx_data_len     = 0U;
static uint32_t s_mock_instance   = 0x40013000U;  /* SPI1 base address stub */

/* ─── Mock control functions ─────────────────────────────────────────────── */

void mock_spi_reset(void)
{
    s_transmit_error = false;
    s_receive_error  = false;
    s_rx_data_len    = 0U;
    (void)memset(s_rx_data, 0xFF, sizeof(s_rx_data));
}

void mock_spi_set_transmit_error(bool force_error)
{
    s_transmit_error = force_error;
}

void mock_spi_set_receive_error(bool force_error)
{
    s_receive_error = force_error;
}

void mock_spi_set_rx_data(const uint8_t *data, uint16_t len)
{
    if ((data != NULL) && (len <= 256U))
    {
        (void)memcpy(s_rx_data, data, (size_t)len);
        s_rx_data_len = len;
    }
}

uint32_t mock_spi_get_instance(void)
{
    return s_mock_instance;
}

/* ─── Mock HAL function implementations ─────────────────────────────────── */

HAL_StatusTypeDef HAL_SPI_Transmit(SPI_HandleTypeDef *hspi,
                                    const uint8_t     *pData,
                                    uint16_t           Size,
                                    uint32_t           Timeout)
{
    (void)hspi;
    (void)pData;
    (void)Size;
    (void)Timeout;

    return (s_transmit_error == true) ? HAL_ERROR : HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_Receive(SPI_HandleTypeDef *hspi,
                                   uint8_t           *pData,
                                   uint16_t           Size,
                                   uint32_t           Timeout)
{
    (void)hspi;
    (void)Timeout;

    if (s_receive_error == true)
    {
        return HAL_ERROR;
    }

    if ((pData != NULL) && (s_rx_data_len > 0U))
    {
        uint16_t copy_len = (Size < s_rx_data_len) ? Size : s_rx_data_len;
        (void)memcpy(pData, s_rx_data, (size_t)copy_len);
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_SPI_TransmitReceive(SPI_HandleTypeDef *hspi,
                                           const uint8_t     *pTxData,
                                           uint8_t           *pRxData,
                                           uint16_t           Size,
                                           uint32_t           Timeout)
{
    (void)hspi;
    (void)pTxData;
    (void)Timeout;

    if (s_transmit_error == true)
    {
        return HAL_ERROR;
    }

    if ((pRxData != NULL) && (s_rx_data_len > 0U))
    {
        uint16_t copy_len = (Size < s_rx_data_len) ? Size : s_rx_data_len;
        (void)memcpy(pRxData, s_rx_data, (size_t)copy_len);
    }

    return HAL_OK;
}
