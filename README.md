# EV Control & Diagnostics System

> **A production-grade embedded EV ECU firmware built using MNC-standard development practices.**  
> Developed by the BaseSync team using Agile + DevOps, full CI/CD, MISRA-inspired coding standards, SIL/HIL testing, and the complete STM32 HAL ecosystem.
 
---

## What Is This?
 
This project implements a **Electric Vehicle ECU (Electronic Control Unit)** on an STM32 microcontroller. It mirrors the core functionality of real automotive ECUs used in EV powertrain systems.

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
                        SMART EV ECU
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
smart-ev-ecu/
├── .github/
│   ├── ISSUE_TEMPLATE/ 
│   └── workflows/      - CI/CD pipelines 
├── Core/
│   ├── Inc/            - Header files (.h)
│   └── Src/            - Source files (.c)
├── Drivers/            - STM32 HAL
│   └── STM32_HAL/
├── Tests/              - Unity unit tests
├── Simulation/         - Wokwi + BusMaster configs
│   ├── busmaster/ 
│   └── wokwi/
├── Docs/               - Documentation mirrors
├── .cppcheck
├── .gitignore
├── .synk
├── LICENSE             - MIT License
└── README.md           - This File
```
 
---

## Development Workflow
 
```
1. Pick a story from GitHub Projects board
2. Create branch: git checkout -b feature/your-feature
3. Write code following 06_CODING_STANDARDS.md
4. Write unit tests in Tests/
5. Run: cppcheck Core/ && cd Tests/build && ./test_runner
6. Open Pull Request → fill in template
7. CI must be green before merge
8. At least 1 reviewer must approve
```
 
---

## Documentation
 
Full documentation is maintained in Confluence under the **BaseSync** space.
 
 
---

## CAN Message Reference
 
| CAN ID | Frame Name | Period | Content |
|---|---|---|---|
| 0x100 | EV_STATUS | 100ms | Vehicle state, fault flags |
| 0x101 | SENSOR_PACK_1 | 100ms | Batt temp, Motor temp, Speed |
| 0x102 | SENSOR_PACK_2 | 100ms | Voltage, Current, Throttle % |
| 0x1FF | FAULT_FRAME | On-event | Fault code, timestamp |
 
---

## Toolchain
 
| Tool | Version | Purpose |
|---|---|---|
| arm-none-eabi-gcc | 10.3+ | ARM cross-compiler |
| CMake | 3.20+ | Build system |
| Unity | 2.5.2 | Unit test framework |
| Cppcheck | 2.x | Static analysis |
| Wokwi | Latest | SIL simulation |
| BusMaster | 3.x | CAN bus simulation |
| FreeRTOS | 10.x | RTOS (Stage 2) |
| GitHub Actions | — | CI/CD |
| Snyk | Free | Security scanning |
 
---

## Team
 
**Organisation:** [BaseSync](https://github.com/basesync)
 
---
 
## License
 
MIT License — see [LICENSE](LICENSE) for details.
 
---