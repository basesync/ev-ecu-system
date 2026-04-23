# EV ECU Control & Diagnostics System

| |  |
|:---|:---|
| **Organisation** | [basesync](https://github.com/basesync) |
| **Project Version** | v1.0.0 |
| **Last Updated** | 2026 |
| **Status** | ✅ Approved  |

---

This is the official technical documentation for the **EV ECU System** - a firmware system built on an STM32 microcontroller that mimics the core functions of a real Electric Vehicle's Electronic Control Unit.

The system reads sensors, controls motor output, detects faults, communicates over CAN bus, and logs diagnostics over UART - built to automotive industry standards from the ground up.

---

## Quick Links

| I want to… | Go to |
|---|---|
| Understand what this project is | [Project Overview](00_project_overview.md) |
| Set up my development environment | [Environment Setup](01_environment_setup.md) |
| Learn the Git branching workflow | [GitHub Setup](02_github_setup.md) |
| Read what the system must do | [Requirements](03_requirements.md) |
| Understand the system architecture | [System Design](04_system_design.md) |
| See how CI/CD works | [CI/CD Pipelines](05_cicd_pipelines.md) |
| Follow the coding rules | [Coding Standards](06_coding_standards.md) |
| Understand how we test | [Testing Strategy](07_testing_strategy.md) |
| Look up a term | [Glossary](GLOSSARY.md) |

---

## Project Stages

| Stage | Description | Sprint |
|---|---|---|
| **SIL** | Software-in-the-Loop — full simulation on Wokwi, no hardware | 1–4 |
| **Hardware Bring-Up** | Flash firmware to real STM32, fix hardware bugs | 5 |
| **HIL** | Hardware-in-the-Loop — real STM32, simulated sensors via BusMaster | 6 |
| **RTOS** | Port to FreeRTOS, add bootloader, build GUI *(stretch)* | TBD |

---

## Technology Stack

`Embedded C (C99)` · `STM32` · `CMake` · `FreeRTOS` · `Unity` · `Cppcheck` · `GitHub Actions` · `Wokwi` · `BusMaster` · `Teleplot`

---

## Document Index

| Document ID | Title | Version |
|---|---|---|
| — | Project Overview | v1.0.0 |
| 02_environment_setup | Environment Setup | v1.0.0 |
| 03_requirements | Requirements Specification | v1.0 |
| 04_system_design | System Design | v1.0 |
| 05_cicd_pipelines | CI/CD Pipelines | v1.0 |
| 06_coding_standards | Coding Standards | v1.0 |
| 07_coding_standards | Testing Strategy | v1.0 |

---

*Last updated: 2026 · basesync organisation*
