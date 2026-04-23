# EV Control & Diagnostics System

> **A production-grade Electric Vehicle ECU firmware built on STM32 using MNC-standard Agile DevOps practices.**
> Complete with CI/CD, static analysis, unit testing, SIL simulation, and HIL testing - developed by the BaseSync team.

---

## What Is This?

This project implements a **EV Electronic Control Unit (ECU)** firmware for the STM32 microcontroller. It mirrors the core functionality of real automotive ECUs used in electric vehicle powertrain systems.

The project follows industry-standard development practices:
- **Agile Scrum** with 2-week sprints tracked on GitHub Projects
- **Layered firmware architecture** (Application -> Service -> HAL)
- **Automated CI/CD** on every Pull Request via GitHub Actions
- **MISRA-C inspired** coding standards
- **SIL -> HIL** testing workflow used in automotive industry

---

## System Overview

```
 ------------------------------------------------------------------
│                         EV ECU SYSTEM                            │
│                                                                  │
│  INPUTS              PROCESSING                OUTPUTS           │
│  -------             -----------               -------           │
│  Battery Temp -----> Thermal Monitor ------> Fault Code          │
│  Motor Temp   -----> Fault Manager   ------> Safe State          │
│  Current      -----> SOC Estimator   ------> CAN Frames          │
│  Voltage      -----> State Machine   ------> Motor PWM           │
│  Speed        -----> Motor Control   ------> UART Logs           │
│  Throttle     -----> Control Loop    ------> Status LED          │
│  Brake Switch -----> Safety Logic                                │
│  Fault Switch -----> Fault Injection                             │
│                                                                  │
│  ----------------------- CAN Bus -----------------------         │
│  EV_STATUS(0x100) │ SENSOR_PACK(0x101,0x102) │ FAULT(0x1FF)      │
 ------------------------------------------------------------------
```

---

### Features

| Feature | Status |
|---|---|
| Multi-sensor reading (8 sensors) | PLANNED |
| Motor PWM control loop | PLANNED |
| Fault detection & safe state | PLANNED |
| CAN Bus communication | PLANNED |
| UART data logging (Teleplot format) | PLANNED |
| EV state machine (5 states) | PLANNED |
| Watchdog timer | PLANNED |
| Wokwi SIL simulation | PLANNED |
| Unity unit tests (>80% coverage) | PLANNED |
| GitHub CI/CD pipeline | PLANNED |
| FreeRTOS port | 🔵 Stage 2 |
| Bootloader | 🔵 Stage 2 |
| GUI Dashboard | 🔵 Stage 2 |

---

## System Architecture

```
                            EV ECU
          -----------------------------------------
         │           APPLICATION LAYER             │
         │  EV State Machine │ Fault Manager       │
          -----------------------------------------
         │            SERVICE LAYER                │
         │  Sensor HAL │ Motor Ctrl │ CAN │ Logger │
          -----------------------------------------
         │           STM32 HAL / BSP               │
         │  ADC │ GPIO │ TIM(PWM) │ CAN │ UART     │
          -----------------------------------------
              │           │          │         │
         [Sensors]    [Motor]    [CAN Bus]  [UART/PC]
```

### Sensors

| Sensor | STM32 Pin | Sampling Rate |
|---|---|---|
| Battery Temperature | PA0 (ADC) | 100ms |
| Motor Temperature | PA1 (ADC) | 100ms |
| Battery Current | PA2 (ADC) | 50ms |
| Battery Voltage | PA3 (ADC) | 50ms |
| Motor Speed | PA6/PA7 (Encoder) | 10ms |
| Throttle | PA4 (ADC) | 10ms |
| Brake Switch | PB0 (GPIO) | 10ms |
| Fault Switch | PB1 (GPIO) | 10ms |

---

## Project Structure

```
ev-ecu-system/
├── .github/
│   ├── workflows/             - CI/CD pipeline definitions
│   │   ├── build.yml          - Firmware compile check
│   │   ├── unit-tests.yml     - Unity test runner
│   │   ├── static-analysis.yml- Cppcheck analysis
│   │   ├── security.yml       - Snyk + CodeQL scanning
│   │   └── release.yml        - Automated release builds
│   ├── ISSUE_TEMPLATE/        - Bug report, feature request templates
│   ├── PULL_REQUEST_TEMPLATE.md
│   └── dependabot.yml         - Automated dependency updates
├── core/
│   ├── Inc/                   - Header files (.h)
│   │   ├── ev_types.h         - Shared type definitions
│   │   ├── ev_config.h        - All constants and thresholds
│   │   ├── sensor_hal.h       - (Sprint 2)
│   │   ├── motor_ctrl.h       - (Sprint 2)
│   │   ├── fault_manager.h    - (Sprint 3)
│   │   ├── can_driver.h       - (Sprint 3)
│   │   ├── logger.h           - (Sprint 4)
│   │   └── ev_state_machine.h - (Sprint 4)
│   └── src/                   - Source files (.c)
│       └── main.c             - Firmware entry point
├── Drivers/
│   └── STM32_HAL/             - STM32 HAL library (added Sprint 2)
├── Tests/
│   ├── Unity/                 - Unity test framework source
│   ├── mocks/                 - STM32 HAL stub functions
│   ├── CMakeLists.txt         - Test build configuration
│   ├── test_runner.c          - Test entry point
│   └── test_placeholder.c     - Sprint 1 framework verification
├── simulation/
│   └── wokwi/
│       ├── diagram.json       - Virtual circuit definition
│       └── wokwi.toml         - Wokwi project config
├── cmake/
│   └── arm-gcc-toolchain.cmake- Cross-compilation configuration
├── .vscode/                   - Shared VS Code configuration
├── CMakeLists.txt             - Root build configuration
├── .cppcheck                  - Static analysis configuration
├── .clang-format              - Code formatting rules
└── .editorconfig              - Editor consistency settings
```

---

## Quick Start

### Prerequisites

| Tool | Version | Purpose |
|---|---|---|
| `arm-none-eabi-gcc` | 10.3+ | ARM cross-compiler |
| `cmake` | 3.20+ | Build system |
| `make` | 4.x | Build runner |
| `cppcheck` | 2.x | Static analysis |
| `git` | 2.x | Version control |
| VS Code | Latest | IDE |

### Clone and Build

```bash
# Clone
git clone git@github.com:basesync/ev-ecu-system.git
cd ev-ecu-system

# Build firmware
cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake
cmake --build build

# Run unit tests (no hardware needed)
cmake -B Tests/build -S Tests && cmake --build Tests/build
./Tests/build/test_runner

# Run static analysis
cppcheck --enable=warning --error-exitcode=1 -I core/Inc core/src/
```

### Run in Simulation (No Hardware Required)

1. Install [Wokwi VS Code extension](https://marketplace.visualstudio.com/items?itemName=wokwi.wokwi-vscode)
2. Get your [free Wokwi license](https://wokwi.com/license)
3. Build firmware (step above)
4. Open `simulation/wokwi/wokwi.toml` in VS Code
5. Press `F1` -> `Wokwi: Start Simulator`

You'll see a virtual STM32 with potentiometers and buttons — turn the throttle, press the brake, trigger faults.

---

## CAN Bus Message Reference

| CAN ID | Frame Name | Transmission | Content |
|---|---|---|---|
| `0x100` | `EV_STATUS` | Every 100ms | Vehicle state, fault flags, SOC |
| `0x101` | `SENSOR_PACK_1` | Every 100ms | Battery temp, motor temp, speed |
| `0x102` | `SENSOR_PACK_2` | Every 100ms | Voltage, current, throttle% |
| `0x1FF` | `FAULT_FRAME` | On fault event | Fault code, timestamp |

---

## Development Workflow

```
1. Pick a story from GitHub Projects board
2. Create branch: git checkout -b feature/your-feature
3. Write code following 06_CODING_STANDARDS.md
4. Write unit tests in Tests/
5. Run: cppcheck core/ && cd Tests/build && ./test_runner
6. Open Pull Request -> fill in template
7. CI must be green before merge
8. At least 1 reviewer must approve
```
---

## Documentation

Full documentation is maintained in Confluence under the [BaseSync Confluence Space](https://basesync.atlassian.net/wiki/spaces/BSYNC/overview).


---

## CAN Message Reference

| CAN ID | Frame Name | Period | Content |
|---|---|---|---|
| 0x100 | EV_STATUS | 100ms | Vehicle state, fault flags |
| 0x101 | SENSOR_PACK_1 | 100ms | Batt temp, Motor temp, Speed |
| 0x102 | SENSOR_PACK_2 | 100ms | Voltage, Current, Throttle % |
| 0x1FF | FAULT_FRAME | On-event | Fault code, timestamp |

---

## Team

**Organisation:** [BaseSync](https://github.com/basesync)

---

## Project Status

**Current Sprint:** Sprint 1 - Foundation & Setup
**Sprint Goal:** Working development environment + full CI/CD pipeline on every PR

---

## License

MIT License — see [LICENSE](LICENSE) for details.

---
