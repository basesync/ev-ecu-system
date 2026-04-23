/**
 * @file    mock_stm32_hal_i2c.c
 * @brief   Mock STM32 I2C HAL implementation for unit testing
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 2 — minimal stub for compilation)
 */

#include "mock_stm32_hal_i2c.h"
#include <string.h>
#include <stdbool.h>

/* ─── Mock state ─────────────────────────────────────────────────────────── */
static bool              s_device_present = true;
static uint8_t           s_rx_data[32];
static uint16_t          s_rx_data_len   = 0U;
static I2C_HandleTypeDef s_mock_handle   = { .Instance = 0x40005400U };

/* ─── Mock control functions ─────────────────────────────────────────────── */

void mock_i2c_reset(void)
{
    s_device_present = true;
    s_rx_data_len    = 0U;
    (void)memset(s_rx_data, 0, sizeof(s_rx_data));
}

void mock_i2c_set_device_ready(bool device_present)
{
    s_device_present = device_present;
}

void mock_i2c_set_rx_data(const uint8_t *data, uint16_t len)
{
    if ((data != NULL) && (len <= 32U))
    {
        (void)memcpy(s_rx_data, data, (size_t)len);
        s_rx_data_len = len;
    }
}

I2C_HandleTypeDef *mock_i2c_get_handle(void)
{
    return &s_mock_handle;
}

/* ─── Mock HAL function implementations ─────────────────────────────────── */

HAL_StatusTypeDef HAL_I2C_IsDeviceReady(I2C_HandleTypeDef *hi2c,
                                          uint16_t           DevAddress,
                                          uint32_t           Trials,
                                          uint32_t           Timeout)
{
    (void)hi2c;
    (void)DevAddress;
    (void)Trials;
    (void)Timeout;

    return (s_device_present == true) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef HAL_I2C_Master_Transmit(I2C_HandleTypeDef *hi2c,
                                            uint16_t           DevAddress,
                                            uint8_t           *pData,
                                            uint16_t           Size,
                                            uint32_t           Timeout)
{
    (void)hi2c; (void)DevAddress; (void)pData; (void)Size; (void)Timeout;
    return (s_device_present == true) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef HAL_I2C_Master_Receive(I2C_HandleTypeDef *hi2c,
                                           uint16_t           DevAddress,
                                           uint8_t           *pData,
                                           uint16_t           Size,
                                           uint32_t           Timeout)
{
    (void)hi2c; (void)DevAddress; (void)Timeout;

    if ((s_device_present == false) || (pData == NULL))
    {
        return HAL_ERROR;
    }

    if (s_rx_data_len > 0U)
    {
        uint16_t copy_len = (Size < s_rx_data_len) ? Size : s_rx_data_len;
        (void)memcpy(pData, s_rx_data, (size_t)copy_len);
    }

    return HAL_OK;
}

HAL_StatusTypeDef HAL_I2C_Mem_Write(I2C_HandleTypeDef *hi2c,
                                      uint16_t           DevAddress,
                                      uint16_t           MemAddress,
                                      uint16_t           MemAddSize,
                                      uint8_t           *pData,
                                      uint16_t           Size,
                                      uint32_t           Timeout)
{
    (void)hi2c; (void)DevAddress; (void)MemAddress;
    (void)MemAddSize; (void)pData; (void)Size; (void)Timeout;
    return (s_device_present == true) ? HAL_OK : HAL_ERROR;
}

HAL_StatusTypeDef HAL_I2C_Mem_Read(I2C_HandleTypeDef *hi2c,
                                     uint16_t           DevAddress,
                                     uint16_t           MemAddress,
                                     uint16_t           MemAddSize,
                                     uint8_t           *pData,
                                     uint16_t           Size,
                                     uint32_t           Timeout)
{
    (void)hi2c; (void)DevAddress; (void)MemAddress;
    (void)MemAddSize; (void)Timeout;

    if ((s_device_present == false) || (pData == NULL))
    {
        return HAL_ERROR;
    }

    if (s_rx_data_len > 0U)
    {
        uint16_t copy_len = (Size < s_rx_data_len) ? Size : s_rx_data_len;
        (void)memcpy(pData, s_rx_data, (size_t)copy_len);
    }

    return HAL_OK;
}
