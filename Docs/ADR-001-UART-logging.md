# ADR-001 — UART for Sensor Logging

**Status:** Decided  
**Date:** Sprint 2  
**Decision makers:** Full team  
**Document ID:** BASESYNC-ADR-001

---

## Context

The system needs to output sensor data for real-time monitoring during development, simulation testing, and hardware bring-up. During Sprint 1–4 (simulation), `printf()` stubs are used in `main.c`. In Sprint 5, real UART output is needed on physical hardware.

## Decision

Use **USART1** at **115200 baud, 8N1** for all sensor logging.  
Pins: TX = PA9, RX = PA10.  
Output format: **Teleplot-compatible** strings (`>label:value\n`) — readable by the VS Code Teleplot extension and any serial terminal.

## Rationale

| Option | Verdict | Reason |
|---|---|---|
| USART1 at 115200 | ✅ Chosen | Teleplot default baud rate, USB-Serial adapters support it universally |
| SWO (Serial Wire Output) | ❌ Rejected | Requires ST-Link debug connection at all times — unusable in vehicle |
| CAN bus logging | ❌ Rejected | Adds traffic on the vehicle bus; BusMaster already busy with ECU frames |
| USB CDC (virtual COM) | ❌ Rejected | More complex driver, requires USB clock setup, overkill for logging |

## Implementation Plan

| Sprint | Action |
|---|---|
| Sprint 1–4 | `printf()` stubs in `main.c` — CI-safe, no real UART |
| Sprint 5 | CubeMX configures USART1 → `logger.c` created using `HAL_UART_Transmit()` |
| Sprint 5+ | Consider DMA TX to avoid blocking main loop on long log lines |

## Affected Files

| File | Change |
|---|---|
| `Core/Inc/ev_config.h` | `EV_UART_BAUD_RATE`, `EV_UART_LOG_LINE_MAX_LEN`, `EV_UART_TX_TIMEOUT_MS` added Sprint 2 |
| `Core/Src/main.c` | TODO comment at exact Sprint 5 integration point |
| `Drivers/STM32_HAL/stm32f1xx_hal_uart.h` | Stub header added Sprint 2 |
| `Core/Inc/logger.h` | New module header — Sprint 4 |
| `Core/Src/logger.c` | New module — Sprint 4 (stub), Sprint 5 (real UART) |

## Consequences

- No other module changes when UART is activated in Sprint 5.
- `logger.c` encapsulates all UART access — fault_manager, state_machine never call HAL_UART directly.
- If UART fails to init in Sprint 5, system continues safely without logging (UART is non-critical).
