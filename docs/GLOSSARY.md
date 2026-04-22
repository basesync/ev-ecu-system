# Glossary

GitBook renders this file as hover-over definitions throughout all pages. Terms are listed alphabetically.

---

## A

**ADC (Analogue-to-Digital Converter)**
A peripheral on the STM32 that converts an analogue voltage signal (e.g. from a temperature sensor or potentiometer) into a digital value the firmware can process. Used to read battery temperature, motor temperature, voltage, current, and throttle position.

**AUTOSAR (AUTomotive Open System ARchitecture)**
An industry-wide standard for automotive software architecture. This project uses an AUTOSAR-inspired 3-layer design (HAL → Service → Application) without fully implementing the standard.

## B

**BusMaster**
An open-source CAN bus simulation and analysis tool. Used in this project to simulate the vehicle CAN network during SIL and HIL testing, and to inject test frames.

**Bare-Metal**
Firmware that runs directly on hardware without an operating system or RTOS. The initial implementation of this project is bare-metal before being ported to FreeRTOS.

## C

**CAN Bus (Controller Area Network)**
A robust vehicle bus standard that allows microcontrollers and devices to communicate without a host computer. Used in this project for inter-module communication between the ECU and other vehicle systems. Runs at 500 kbps with standard 11-bit identifiers.

**CI/CD (Continuous Integration / Continuous Deployment)**
Automated pipelines that build, test, and release code on every push or pull request. Implemented via GitHub Actions in this project. Nothing merges to `main` or `develop` without CI passing.

**CMake**
A cross-platform build system generator. Used to configure and build the STM32 firmware and the native unit test suite. The ARM cross-compilation toolchain is specified via `cmake/arm-gcc-toolchain.cmake`.

**Cppcheck**
A static analysis tool for C/C++ code. It detects bugs, undefined behaviour, and style issues without executing the code. Run automatically in CI and enforced as a PR gate.

## D

**Dependabot**
A GitHub feature that automatically opens pull requests when GitHub Actions dependencies are outdated.

**DMA (Direct Memory Access)**
A hardware feature allowing peripherals (such as the ADC) to write data directly to memory without CPU intervention. Used for ADC scanning in this project to reduce CPU load.

**Doxygen**
A documentation generator for C/C++ code. Comments using `@brief`, `@param`, and `@retval` tags in this project are Doxygen-format and can be used to auto-generate API documentation.

## E

**ECU (Electronic Control Unit)**
An embedded computer in a vehicle that reads sensors, makes decisions, and controls actuators. The central artifact of this project.

**Encoder**
A sensor attached to the motor shaft that outputs pulses as it rotates. The STM32 reads these pulses in encoder interface mode (TIM3) to calculate motor/wheel speed in RPM.

**EV (Electric Vehicle)**
A vehicle propelled by one or more electric motors. The simulated domain of this project.

## F

**FreeRTOS**
A popular open-source Real-Time Operating System for embedded systems. Used in Stage 4 of this project to port the bare-metal firmware to a task-based concurrent architecture.

**Fault Code**
A numeric identifier representing a specific detected fault condition. Stored in non-volatile memory (simulated EEPROM/Flash) and transmitted over CAN in a `FAULT_FRAME` (CAN ID `0x1FF`).

## G

**GCC (GNU Compiler Collection)**
The compiler used to build this project. Two versions are used: `arm-none-eabi-gcc` for cross-compiling firmware for the STM32, and native `gcc` for compiling unit tests to run on a PC.

**GPIO (General Purpose Input/Output)**
A configurable digital pin on the STM32. Used in this project to read the brake switch and fault trigger switch states.

## H

**HAL (Hardware Abstraction Layer)**
The lowest layer of the 3-layer architecture. Wraps all direct hardware access (ADC reads, GPIO reads, PWM writes, CAN and UART transmit) so that the layers above can be tested without real hardware.

**HIL (Hardware-In-the-Loop)**
A testing methodology where real embedded hardware runs the firmware, but the sensors and actuators are replaced by a controlled simulation (in this project, BusMaster + test GPIO). Stage 3 of the project.

## I

**IWDG (Independent Watchdog)**
A hardware timer on the STM32 that resets the MCU if the firmware fails to periodically "feed" it within the timeout period (500ms in this project). Used as a safety mechanism to recover from firmware hangs.

## L

**LM35**
An analogue temperature sensor whose output voltage is directly proportional to temperature in Celsius (10mV/°C). Used as the simulated temperature sensor model in this project.

## M

**MISRA-C**
A set of C language coding guidelines developed by the Motor Industry Software Reliability Association. Designed for safety-critical embedded systems. This project follows a simplified subset of MISRA-C rules (MISRA-S1 through MISRA-S6).

**MCU (Microcontroller Unit)**
A compact integrated circuit containing a processor, memory, and programmable I/O peripherals. The target hardware in this project is an STM32 MCU.

## P

**PWM (Pulse Width Modulation)**
A technique for simulating an analogue output by rapidly switching a digital signal on and off. The ratio of on-time to period is the duty cycle (0–100%). Used in this project to control motor speed via TIM1 CH1.

## R

**Renode**
An open-source hardware simulation framework that can run unmodified firmware on a virtual MCU. An alternative to Wokwi for SIL testing.

**RTM (Requirements Traceability Matrix)**
A table linking each requirement to the test case(s) that verify it. Maintained in `BASESYNC-REQ-001.md`.

**RTOS (Real-Time Operating System)**
An operating system designed to handle tasks with strict timing requirements. FreeRTOS is the RTOS targeted in Stage 4 of this project.

## S

**SAFE_STATE**
The system state entered whenever a critical fault is detected. In `SAFE_STATE`, motor PWM is set to 0% and a `FAULT_FRAME` is transmitted over CAN. The system stays in this state until an explicit fault clear command is received.

**SIL (Software-In-the-Loop)**
A testing methodology where the complete firmware runs inside a software simulator (Wokwi/Renode) with no physical hardware. Stage 1 of the project.

**Snyk**
A security scanning tool that checks for known vulnerabilities in project dependencies. Run automatically in CI.

**State Machine**
The top-level control logic of the ECU firmware. Transitions between `INIT`, `IDLE`, `RUNNING`, and `SAFE_STATE` based on sensor data and fault conditions.

**STM32**
A family of 32-bit ARM Cortex-M microcontrollers made by STMicroelectronics. The target hardware for this project (specifically STM32F103 or F4 series).

**STM32CubeIDE**
ST Microelectronics' official IDE, integrating CubeMX for pin configuration and HAL code generation. Used in this project for initial HAL setup before switching to VS Code for development.

## T

**Teleplot**
A VS Code extension and serial plotter that parses `>label:value` formatted UART output and renders it as a live graph. Used in this project for real-time sensor data visualisation.

**TJA1050**
A CAN bus transceiver IC that interfaces between the STM32's CAN controller and the physical CAN bus differential pair.

## U

**UART (Universal Asynchronous Receiver-Transmitter)**
A serial communication protocol. Used in this project to transmit Teleplot-formatted sensor data and fault logs to a connected PC at 115200 baud, 8N1.

**Unity**
A lightweight C unit testing framework. Used in this project to write and run all unit tests. Produces structured output showing tests passed, failed, and ignored.

## W

**Watchdog**
See *IWDG*.

**Wokwi**
A free, browser-based electronics simulator supporting STM32 microcontrollers. Used for SIL testing in this project — virtual potentiometers simulate sensors, virtual buttons simulate switches, and virtual LEDs represent PWM output.

---

*Terms are updated as the project evolves. Submit a PR to add missing terms.*
