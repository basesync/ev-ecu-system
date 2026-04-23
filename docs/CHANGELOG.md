# Changelog

All notable changes to the **BaseSync EV ECU System** firmware are documented in this file.

This project follows [Semantic Versioning](https://semver.org/spec/v2.0.0.html) and the
[Keep a Changelog](https://keepachangelog.com/en/1.0.0/) format.

Each version maps directly to one sprint.

---

## [0.2.0] — Sprint 2: Sensor HAL · Motor Control · Protocol Preparation

> **Sprint Goal:** Sensor HAL reads all 8 sensor inputs and motor control generates
> correct PWM. Both modules are unit-tested and demonstrated in Wokwi simulation.
> Protocol integration preparation (UART / I2C / SPI) completed with stubs and
> ADRs in place for Sprint 5 and Sprint 6.

### Added

#### `core/inc/sensor_hal.h` · `core/src/sensor_hal.c`
- New module: `sensor_hal` — Hardware Abstraction Layer for all 8 sensors
- `sensor_init(hadc, htim)` — store ADC1 and TIM3 handles; start encoder mode
- `sensor_read_all(sensor_data_t*)` — read all 8 sensors into struct in one call
- `sensor_read_batt_temp()` — ADC CH0 (PA0) · LM35 · `voltage / 0.01` = °C
- `sensor_read_motor_temp()` — ADC CH1 (PA1) · LM35
- `sensor_read_current()` — ADC CH2 (PA2) · ACS712-20A · `(V − 2.5) / 0.1` = Amperes
- `sensor_read_voltage()` — ADC CH3 (PA3) · resistor divider R1=100kΩ R2=10kΩ · ratio × 11
- `sensor_read_speed()` — TIM3 encoder mode (PA6/PA7) · 2400 PPR · returns RPM
- `sensor_read_throttle()` — ADC CH4 (PA4) · 10kΩ potentiometer · 0–100%
- `sensor_read_brake()` — GPIO PB0 · active-low · 20 ms software debounce
- `sensor_read_fault_switch()` — GPIO PB1 · active-low · 20 ms software debounce
- `sensor_enable_i2c_temp(hi2c)` — stub; activates TMP102 I2C path in Sprint 5
- `sensor_get_temp_backend()` — returns `EV_PROTO_STATUS_STUB` until Sprint 5

#### `core/inc/motor_ctrl.h` · `core/src/motor_ctrl.c`
- New module: `motor_ctrl` — PWM motor speed control
- `motor_init(htim)` — store TIM1 handle; start PWM at 0% duty on PA8
- `motor_set_speed(pct)` — set TIM1 CH1 compare register; 0–100%; deadband applied
- `motor_stop()` — immediate PWM cutoff; always returns `EV_STATUS_OK`
- `motor_soft_start(target)` — ramp duty 1% per step over ~500 ms
- `motor_get_speed()` — return last commanded speed percentage
- TIM1 CH1, PA8, 20 kHz, ARR = 3599 at 72 MHz

#### `core/inc/fault_logger.h` · `core/src/fault_logger.c` (stub)
- Interface defined for Sprint 6 SPI flash fault storage
- `fault_logger_init()`, `fault_logger_write()`, `fault_logger_read_last()`, `fault_logger_get_count()`, `fault_logger_clear_all()`, `fault_logger_get_backend_status()`
- All functions return safe defaults; no flash I/O until Sprint 6
- `fault_logger_get_backend_status()` returns `EV_PROTO_STATUS_STUB`

#### `core/inc/ev_config.h` — updated v1.2
- Section 9 — UART: `EV_UART_BAUD_RATE` (115200), `EV_UART_LOG_LINE_MAX_LEN`, TX buffer constants
- Section 10 — I2C: TMP102 address (`0x48`), register map, `EV_I2C_SPEED_HZ` (400 kHz), `EV_I2C_TMP102_DEG_PER_LSB` (0.0625)
- Section 11 — SPI: W25Q32 JEDEC ID (`0xEF4016`), command bytes, flash geometry, CS pin (`PA4`)

#### `core/inc/ev_types.h` — updated v1.1
- `fault_log_entry_t` struct — 8 bytes: `fault_code`, `reserved`, `crc16`, `timestamp_ms`
- `ev_proto_status_t` enum — `NOT_USED (0)`, `STUB (1)`, `ACTIVE (2)`

#### `drivers/STM32_HAL/` — stub headers added
- `stm32f1xx_hal_uart.h` — `HAL_UART_Transmit()`, `HAL_UART_Receive()`, `HAL_UART_Transmit_DMA()`
- `stm32f1xx_hal_i2c.h` — `HAL_I2C_Master_Transmit/Receive()`, `HAL_I2C_Mem_Read/Write()`, `HAL_I2C_IsDeviceReady()`
- `stm32f1xx_hal_spi.h` — `HAL_SPI_Transmit()`, `HAL_SPI_Receive()`, `HAL_SPI_TransmitReceive()`

#### `drivers/STM32_HAL/` — stub `.c` implementations added
- `stm32f1xx_hal_gpio.c` — `HAL_GPIO_ReadPin()` returns `GPIO_PIN_SET`; `HAL_GetTick()` returns 0; `HAL_Delay()` is no-op
- `stm32f1xx_hal_adc.c` — `HAL_ADC_GetValue()` returns 0
- `stm32f1xx_hal_tim.c` — `HAL_TIM_PWM_Start()` and `HAL_TIM_Encoder_Start()` set `htim->Instance` to static `TIM_TypeDef` (prevents null-dereference in `__HAL_TIM_SET_COMPARE` macro)
- `stm32f1xx_hal_uart.c` — `HAL_UART_Transmit()` discards data; returns `HAL_OK`
- `stm32f1xx_hal_i2c.c` — `HAL_I2C_IsDeviceReady()` returns `HAL_ERROR` (keeps sensor in ADC mode); `HAL_I2C_Mem_Read()` zeroes buffer
- `stm32f1xx_hal_spi.c` — `HAL_SPI_TransmitReceive()` fills buffer with `0xFF` (JEDEC mismatch → fault logger stays stub)

#### Tests
- `tests/mocks/mock_stm32_hal_adc.c/.h` — `mock_adc_set_channel_value()`, `mock_adc_set_start_error()`, `mock_adc_set_poll_timeout()`
- `tests/mocks/mock_stm32_hal_tim.c/.h` — `mock_tim_set_encoder_count()`, `mock_tim_get_compare_value()`, `mock_tim_get_instance()`
- `tests/mocks/mock_stm32_hal_gpio.c/.h` — `mock_gpio_set_pin()`, `mock_gpio_get_written_pin()`, `mock_hal_set_tick()`
- `tests/mocks/mock_stm32_hal_i2c.c/.h` — `mock_i2c_set_device_ready()`, `mock_i2c_set_rx_data()`
- `tests/mocks/mock_stm32_hal_spi.c/.h` — `mock_spi_set_rx_data()`, `mock_spi_set_transmit_error()`
- `tests/test_sensor_hal.c` — 22 unit tests (ADC conversions, boundaries, debounce, NULL guards)
- `tests/test_motor_ctrl.c` — 18 unit tests (PWM duty, NULL handle, deadband, stop, speed tracking)
- `tests/test_protocol_prep.c` — 26 unit tests (struct layout, constants, stub return values)
- `tests/test_runner.c` — updated; 66 new `RUN_TEST()` registrations
- `tests/CMakeLists.txt` — all new mock and firmware sources added

**Total unit tests: 67 (0 failures)**

#### CI/CD
- `.github/workflows/coverage.yml` — added coverage reporting with `lcov`; HTML report uploaded as CI artifact; Codecov integration

#### Simulation
- `simulation/wokwi/diagram.json` — updated; 5 ADC potentiometers, motor PWM LED, status LED, brake button, fault button wired to correct Blue Pill pins

#### Documentation
- `docs/ADR-001-UART-logging.md` — architecture decision record for UART Teleplot logging approach
- `docs/ADR-002-003-I2C-SPI.md` — ADRs for TMP102 I2C sensor and W25Q32 SPI flash integration strategy

### Changed
- `core/src/main.c` — v0.2.0: sensor and motor HAL initialised; main loop reads all sensors; brake override logic; UART Teleplot `printf()` stubs every 100 ms
- `CMakeLists.txt` — `sensor_hal.c`, `motor_ctrl.c`, `fault_logger.c` added to `FIRMWARE_SOURCES`; version bumped to `0.2.1`

### Known Limitations
- Fault detection not implemented — Sprint 3
- EV state machine not implemented — Sprint 3
- CAN bus not transmitting — Sprint 3
- `printf()` output is a stub — real `HAL_UART_Transmit()` in Sprint 5
- I2C TMP102 not connected — Sprint 5
- SPI W25Q32 not connected — Sprint 6
- `HAL_GetTick()` returns 0 (stub) — GPIO debounce timer never advances; brake/fault switches always read released in non-Wokwi stub builds

---

## [0.1.0] — Sprint 1: Foundation & Setup

> **Sprint Goal:** Every team member has a working development environment, the
> repository is fully configured with branch protection, and the CI/CD pipeline
> runs all three jobs on every pull request.

### Added

#### Repository structure
- `core/inc/` · `core/src/` · `drivers/STM32_HAL/` · `tests/` · `simulation/wokwi/` · `cmake/` · `docs/`
- `.gitignore` - ARM GCC, CMake build artefacts, IDE files
- `.editorconfig` - 4-space indent, LF line endings, final newline enforced
- `.clang-format` - Google style, column limit 100
- `.cppcheck` - project-level suppressions for Cppcheck

#### Firmware skeleton
- `core/inc/ev_types.h` v1.0 - `ev_status_t`, `ev_state_t`, `fault_code_t`, `sensor_data_t`, `ev_status_t` struct
- `core/inc/ev_config.h` v1.0 - fault thresholds, timing constants, CAN IDs, ADC constants, PWM config
- `core/src/main.c` - bare `main()` entry point; onboard LED (PC13) blinks on 500 ms loop; `delay_ms()` software stub
- `CMakeLists.txt` - ARM GCC cross-compilation; `smart_ev_ecu.elf` target; post-build `.bin` and `.hex` generation; version `0.1.0`
- `cmake/arm-gcc-toolchain.cmake` - `arm-none-eabi-gcc` toolchain file; Cortex-M3 flags; linker script reference

#### GitHub configuration
- `README.md` - project overview, hardware list, quick-start commands, CI badge
- `CONTRIBUTING.md` - branch naming conventions, commit message format, PR checklist, code style rules
- `.github/ISSUE_TEMPLATE/bug_report.md` - structured bug report template
- `.github/ISSUE_TEMPLATE/feature_request.md` - user story template with acceptance criteria
- `.github/PULL_REQUEST_TEMPLATE.md` - PR checklist: build pass, tests pass, Cppcheck pass, description filled
- `.github/dependabot.yml` - weekly GitHub Actions dependency updates
- Branch protection rules on `main` and `develop`: PRs required; all CI checks must pass; no direct push

#### CI/CD workflows
- `.github/workflows/build.yml` - ARM GCC cross-compilation on `ubuntu-22.04`; uploads `.elf` / `.bin` / `.hex` as artifacts; retention 7 days
- `.github/workflows/unit-tests.yml` - native GCC; builds and runs `./test_runner`; fails on any test failure
- `.github/workflows/static-analysis.yml` - Cppcheck `--enable=warning,style,performance,portability`; uploads XML report; `--error-exitcode=1`
- `.github/workflows/security.yml` - Dependabot security scan
- `.github/workflows/release.yml` - triggered on `v*` tag push; creates GitHub release; attaches firmware artifacts

#### Unity test framework
- `tests/test_placeholder.c` - 3 placeholder tests (framework operational, integer assertion, boolean assertion)
- `tests/test_runner.c` - `UNITY_BEGIN()` / `UNITY_END()` runner
- `tests/CMakeLists.txt` - native GCC test build; `--coverage` option for Sprint 4

#### Wokwi simulation skeleton
- `simulation/wokwi/diagram.json` - STM32 Blue Pill + onboard LED + one potentiometer + one push-button
- `simulation/wokwi/wokwi.toml` - links compiled `.elf` to simulation

#### Docs
- `00_project_overview` - Overview of the project.
- `01_environment_setup` - Setting up the environment for project
- `02_github_setup` - Setting up guthub (only for team members)
- `03_requirements` - Functional and non-functional requirements
- `04_system_design` - Design of the system
- `05_cicd_pipelines` - Pipelines explained
- `06_coding_standards` - MISRA standards and guidelines for coding
- `07_testing_strategy` - Testing steps and details
- `CHANGELOG.md` - Release details and changes
- `GLOSSARY.md` - Abbrevations and explanations
- `README.md` - Home page for gitbook
- `SUMMARY.md` - Structure for gitbook

**Total unit tests: 3 (0 failures)**

### Known Limitations
- No sensor or motor firmware — Sprint 2
- No fault detection or CAN — Sprints 3–4
- HAL is STM32 toolchain only; unit tests use stub headers — Sprint 5 replaces with CubeMX HAL

---

## Upcoming Releases

| Version | Sprint | Target content |
|---|---|---|
| `v0.2.0` | Sprint 2 | Sensor HAL, Motor Control, Protocol Preparation |
| `v0.3.0` | Sprint 3 | Fault Manager, EV State Machine, CAN Driver |
| `v0.4.0` | Sprint 4 | UART Teleplot logger, SOC estimator, Wokwi SIL verified |
| `v0.5.0` | Sprint 5 | Hardware bring-up: real UART, I2C TMP102, CAN TJA1050, IWDG |
| `v1.0.0` | Sprint 6 | SPI W25Q32 fault logger, HIL test suite, production binary |

---

## Version History Summary

| Version | Sprint | Tests | Flash usage | Notable addition |
|---|---|---|---|---|
| `v0.1.0` | Sprint 1 | 3 | ~2 KB | Project skeleton + CI/CD |
| `v0.2.0` | Sprint 2 | TBD | TBD | Sensor HAL + Motor Control |
| `v0.3.0` | Sprint 3 | TBD | TBD | Fault Manager + State Machine + CAN |
| `v0.4.0` | Sprint 4 | TBD | TBD | UART Logger + SOC Estimator |
| `v0.5.0` | Sprint 5 | TBD | TBD | Hardware Bring-Up |
| `v1.0.0` | Sprint 6 | TBD | TBD | Production Release |

---

## How to Update This File

When a sprint ends and a new release is tagged:

1. Rename `[Unreleased]` to `[X.Y.Z] — Sprint N: Short description`
2. Add the release date after the version: `[0.4.0] — 2025-MM-DD — Sprint 4: ...`
3. Create a new empty `[Unreleased]` section at the top
4. Follow the sections: **Added**, **Changed**, **Fixed**, **Removed**, **Known Limitations**
5. Commit the updated `CHANGELOG.md` as part of the release PR (develop → main)

```
git add CHANGELOG.md
git commit -m "docs(changelog): update for v0.4.0 Sprint 4 release"
```

For a hotfix between sprints:

1. Add a new `[X.Y.Z] — Hotfix` entry between the two sprint entries
2. Use only the **Fixed** section
3. Describe the bug, root cause, and the line changed

---

[0.1.0]: https://github.com/basesync/smart-ev-ecu/releases/tag/v0.1.0
