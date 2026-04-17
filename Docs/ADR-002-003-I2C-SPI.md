# ADR-002 — I2C TMP102 for Battery Temperature Sensing

**Status:** Decided  
**Date:** Sprint 2  
**Document ID:** BASESYNC-ADR-002

---

## Context

Sprints 1–4 simulate battery temperature using a potentiometer on ADC channel 0 (PA0). For Sprint 5 hardware bring-up, a real digital temperature sensor is needed.

## Decision

Replace the ADC potentiometer with a **TMP102 digital temperature sensor** communicating over **I2C1** at **400 kHz** (fast mode).  
Pins: SCL = PB6, SDA = PB7. I2C address: 0x48 (ADD0 pin tied to GND).

## Rationale

| Option | Verdict | Reason |
|---|---|---|
| LM35 on ADC (keep simulation approach) | ❌ Rejected | Requires precise resistor divider and calibration; ADC noise limits accuracy to ±2–5°C |
| TMP102 over I2C | ✅ Chosen | Digital, ±0.5°C accuracy, no calibration needed, 2-wire interface |
| NTC thermistor + ADC | ❌ Rejected | Non-linear response, requires lookup table or Steinhart-Hart equation |
| DS18B20 over 1-Wire | ❌ Rejected | Proprietary protocol, slower (750ms conversion), more complex driver |

## Implementation Impact — Why Only sensor_hal.c Changes

The layered HAL architecture means only the **internal implementation** of `sensor_read_batt_temp()` changes. The function signature stays the same: it always returns a `float` in degrees Celsius. Nothing above it in the stack is affected.

```
fault_manager.c     → calls sensor_read_batt_temp()  ← NO CHANGE
ev_state_machine.c  → calls sensor_read_batt_temp()  ← NO CHANGE
logger.c            → calls sensor_read_batt_temp()  ← NO CHANGE
sensor_hal.c        → internals change: ADC read → I2C read  ← ONLY THIS
```

## Graceful Degradation

If I2C communication fails (device not connected, NAK, timeout):
- `sensor_read_batt_temp()` returns the ADC pot value as fallback
- A UART warning message is logged: `"WARN: I2C TMP102 fail, using ADC fallback"`
- No fault is raised (temperature still readable, just less accurate)
- If both fail: `sensor_read_batt_temp()` returns 0.0f (safe default — no false fault)

## New Functions Declared in Sprint 2

```c
ev_status_t     sensor_enable_i2c_temp(I2C_HandleTypeDef *hi2c);
ev_proto_status_t sensor_get_temp_backend(void);
```

Both declared in `sensor_hal.h` now. Implemented in Sprint 5.

## Hardware Bill of Materials (Sprint 5 Addition)

| Item | Quantity | Purpose |
|---|---|---|
| TMP102 (SOT23-5 or breakout) | 1 | Battery temperature sensor |
| 4.7 kΩ resistor | 2 | I2C pull-ups on SCL and SDA |
| 100 nF capacitor | 1 | Power supply decoupling on TMP102 VCC |

## Affected Files

| File | Change |
|---|---|
| `Core/Inc/ev_config.h` | I2C constants added Sprint 2 |
| `Core/Inc/sensor_hal.h` | New I2C functions declared Sprint 2 |
| `Core/Src/sensor_hal.c` | `sensor_read_batt_temp()` internals + new functions Sprint 5 |
| `Core/Src/main.c` | `sensor_enable_i2c_temp()` call added Sprint 5 |
| `Drivers/STM32_HAL/stm32f1xx_hal_i2c.h` | Stub added Sprint 2 |

---

---

# ADR-003 — SPI W25Q32 Flash for Persistent Fault Log

**Status:** Decided  
**Date:** Sprint 2  
**Document ID:** BASESYNC-ADR-003

---

## Context

Requirements document FR-003-08 states: *"The system SHALL store fault codes in non-volatile memory."*

Currently, `fault_manager.c` stores active fault codes as a bitmask in RAM. These are lost on every power cycle. An automotive ECU must retain fault history for diagnostics — technicians read fault codes after a vehicle incident.

## Decision

Use a **W25Q32 4 MB NOR Flash** chip over **SPI1** for persistent fault log storage.  
Pins: MOSI = PA7, MISO = PA6, SCK = PA5, CS = PA4 (GPIO software-controlled).  
Log area: First 8 KB of flash (first 2 sectors).  
Entry format: 8-byte `fault_log_entry_t` (fault code + timestamp + CRC16).

## Rationale

| Option | Verdict | Reason |
|---|---|---|
| Internal STM32 Flash | ❌ Rejected | Limited erase cycles (10,000); erasing = briefly disabling interrupts |
| External SPI NOR Flash (W25Q32) | ✅ Chosen | 100,000 erase cycles, dedicated chip, no interrupt risk |
| External I2C EEPROM (24C256) | ❌ Rejected | Slower (400 kHz I2C vs 18 MHz SPI), I2C bus shared with TMP102 |
| SD card over SPI | ❌ Rejected | FAT filesystem overhead, slower, higher power draw |

## Why W25Q32 Specifically

- Standard JEDEC SPI NOR flash — command set widely documented
- Available on AliExpress/Mouser for < $1 in SOIC-8 or DIP-8 (easy to hand-solder)
- 4 MB total — far more than the 8 KB needed for fault log (room for future expansion)
- 100,000 sector erase cycles — at one fault per minute, lasts 190 years
- JEDEC ID readable (0xEF4016) — chip presence verifiable on init

## Fault Log Layout in Flash

```
Flash Address 0x000000:
  ┌───────────────────────────────────────┐
  │ Sector 0 (4 KB) — Fault log entries  │
  │   Entry 0: [code][pad][crc16][ts_ms]  │ ← 8 bytes
  │   Entry 1: [code][pad][crc16][ts_ms]  │
  │   ...                                 │
  │   Entry 511: [code][pad][crc16][ts_ms]│
  ├───────────────────────────────────────┤
  │ Sector 1 (4 KB) — Fault log overflow │
  │   Entry 512 ... Entry 1023            │
  ├───────────────────────────────────────┤
  │ Sectors 2+ — Reserved for future use  │
  └───────────────────────────────────────┘
```

## New Module Created in Sprint 2

`fault_logger.h` — full interface declared.  
`fault_logger.c` — stub implementation (all functions return OK, no SPI access).

`fault_manager.c` in Sprint 3 calls `fault_logger_write()` — the stub silently succeeds until Sprint 6 replaces it with real flash writes.

## Hardware Bill of Materials (Sprint 6 Addition)

| Item | Quantity | Purpose |
|---|---|---|
| W25Q32 (SOIC-8 or DIP-8 breakout) | 1 | 4 MB NOR flash for fault storage |
| 100 nF capacitor | 1 | VCC decoupling cap, place close to chip |
| 10 kΩ resistor | 1 | CS pull-up (keeps CS high during STM32 boot) |

## Affected Files

| File | Change |
|---|---|
| `Core/Inc/ev_config.h` | SPI constants added Sprint 2 |
| `Core/Inc/fault_logger.h` | Full interface declared Sprint 2 |
| `Core/Src/fault_logger.c` | Stub Sprint 2, real implementation Sprint 6 |
| `Core/Inc/fault_manager.h` | No change (calls fault_logger via function call) |
| `Core/Src/fault_manager.c` | Calls `fault_logger_write()` Sprint 3 (stub OK) |
| `Core/Src/main.c` | `fault_logger_init()` call added Sprint 6 |
| `Drivers/STM32_HAL/stm32f1xx_hal_spi.h` | Stub added Sprint 2 |

## Consequences

- From Sprint 3 onwards, `fault_manager.c` calls `fault_logger_write()` — stub returns OK, no flash access.
- Sprint 5 and earlier: zero runtime impact — stub is called and returns immediately.
- Sprint 6: Replace stub implementation with real SPI + W25Q32 commands.
- If flash fails on init in Sprint 6: system logs a warning and continues. Fault codes stored in RAM only (limited history). FAULT_FLASH_ERROR bit set in fault_code bitmask.
