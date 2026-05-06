/**
 * @file    mock_stm32_hal_i2c.h
 * @brief   Mock STM32 I2C HAL for unit testing
 *
 * @details Sprint 2: Minimal mock so sensor_enable_i2c_temp() stub
 *          compiles and runs in the test build.
 *          Sprint 5: Expand with full TMP102 register simulation.
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 2 — minimal stub for compilation)
 */

#ifndef MOCK_STM32_HAL_I2C_H
#define MOCK_STM32_HAL_I2C_H

#include "stm32f1xx_hal_i2c.h"
#include <stdbool.h>

/** Reset all I2C mock state. Call in setUp(). */
void mock_i2c_reset(void);

/** Force HAL_I2C_IsDeviceReady to return HAL_ERROR (simulates no device). */
void mock_i2c_set_device_ready(bool device_present);

/** Set bytes returned by HAL_I2C_Master_Receive / HAL_I2C_Mem_Read. */
void mock_i2c_set_rx_data(const uint8_t *data, uint16_t len);

/** Get pointer to a valid I2C_HandleTypeDef for use in tests. */
I2C_HandleTypeDef *mock_i2c_get_handle(void);

#endif /* MOCK_STM32_HAL_I2C_H */
