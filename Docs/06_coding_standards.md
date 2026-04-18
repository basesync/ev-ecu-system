# Coding Standards — EV ECU System

| |  |
|:---|:---|
| **Organisation** | [basesync](https://github.com/basesync) |
| **Project Version** | v1.0.0 |
| **Last Updated** | 2026 |
| **Owner** | [@Rohith-Kalarikkal](https://github.com/Rohith-Kalarikkal) |
| **Status** | ✅ Approved  |

> ⚠️ **These rules are not optional. CI will enforce many of them automatically.**

---

## Table of Contents

1. [Why Coding Standards?](#why-coding-standards)
2. [Naming Conventions](#naming-conventions)
   - [Variables](#variables)
   - [Functions](#functions)
   - [Types — Structs, Enums, Typedefs](#types--structs-enums-typedefs)
   - [Files](#files)
3. [File Structure](#file-structure)
   - [Header File (.h)](#header-file-h-structure)
   - [Source File (.c)](#source-file-c-structure)
4. [Function Rules](#function-rules)
5. [Variable Rules](#variable-rules)
6. [Comments & Documentation](#comments--documentation)
7. [Error Handling](#error-handling)
8. [Header File Guards](#header-file-guards)
9. [Magic Numbers](#magic-numbers)
10. [Memory Usage Rules](#memory-usage-rules)
11. [MISRA-C Inspired Rules](#misra-c-inspired-rules)
12. [Example Compliant Code](#example-compliant-code)

---

## Why Coding Standards?

In a team of 4, everyone writes differently. Without standards:

- You can't read each other's code.
- Code reviews become arguments about style, not logic.
- Bugs hide in inconsistent code.
- Future maintainers (or your future self) will hate you.

---

## Naming Conventions

### Variables

| Type | Convention | Example |
|---|---|---|
| Local variable | `snake_case` | `battery_temp` |
| Global variable | `g_snake_case` | `g_fault_code` |
| Static variable | `s_snake_case` | `s_motor_speed` |
| Constant / `#define` | `UPPER_SNAKE_CASE` | `MAX_BATTERY_TEMP` |
| Enum value | `UPPER_SNAKE_CASE` | `FAULT_OVER_TEMP` |
| Struct member | `snake_case` | `sensor_data.batt_temp` |

---

### Functions

Pattern: `<module>_<verb>_<noun>()`

```c
sensor_read_battery_temp()     // ✅ Good
readBattTemp()                 // ❌ Bad — no module prefix, camelCase
sensor_battery_temperature()   // ❌ Bad — no verb

motor_set_speed(uint8_t pct)   // ✅ Good
fault_check_over_temperature() // ✅ Good
can_send_status_frame()        // ✅ Good
```

---

### Types — Structs, Enums, Typedefs

```c
/* Structs: snake_case_t (always typedef) */
typedef struct {
    float batt_temp;
    float motor_temp;
} sensor_data_t;    /* ✅ Good */

/* Enums: snake_case_t */
typedef enum {
    EV_STATE_INIT,
    EV_STATE_IDLE,
    EV_STATE_RUNNING,
    EV_STATE_FAULT,
    EV_STATE_SAFE
} ev_state_t;

/* Never use raw struct/enum names without typedef */
struct sensor_data data;   /* ❌ Bad */
sensor_data_t data;        /* ✅ Good */
```

---

### Files

```
sensor_hal.c / sensor_hal.h         ← one module per file pair
motor_ctrl.c / motor_ctrl.h
fault_manager.c / fault_manager.h
can_driver.c / can_driver.h
ev_types.h                          ← shared types only, no .c
ev_config.h                         ← all #defines / thresholds
```

---

## File Structure

### Header File (.h) Structure

```c
/**
 * @file    sensor_hal.h
 * @brief   Sensor Hardware Abstraction Layer — public interface
 * @author  BaseSync Team
 * @date    2025
 * @version 1.0
 */

#ifndef SENSOR_HAL_H     /* Header guard — ALWAYS include this */
#define SENSOR_HAL_H

/* ─── Includes ─────────────────────────────────────── */
#include <stdint.h>
#include <stdbool.h>
#include "ev_types.h"

/* ─── Constants ─────────────────────────────────────── */
#define SENSOR_BATT_TEMP_MAX_C    (60.0f)
#define SENSOR_SAMPLE_PERIOD_MS   (100U)

/* ─── Type Definitions ──────────────────────────────── */
typedef struct {
    float    batt_temp;     /**< Battery temperature in °C */
    float    motor_temp;    /**< Motor temperature in °C   */
    float    current;       /**< Battery current in Amps   */
    float    voltage;       /**< Battery voltage in Volts  */
    uint16_t speed_rpm;     /**< Motor speed in RPM        */
    uint8_t  throttle_pct;  /**< Throttle position 0-100%  */
    bool     brake_active;  /**< Brake switch state        */
    bool     fault_switch;  /**< Manual fault switch state */
} sensor_data_t;

/* ─── Function Declarations ─────────────────────────── */
ev_status_t sensor_init(void);
ev_status_t sensor_read_all(sensor_data_t *data);
float       sensor_read_batt_temp(void);
float       sensor_read_motor_temp(void);
float       sensor_read_current(void);
float       sensor_read_voltage(void);
uint16_t    sensor_read_speed(void);
uint8_t     sensor_read_throttle(void);
bool        sensor_read_brake(void);
bool        sensor_read_fault_switch(void);

#endif /* SENSOR_HAL_H */
```

---

### Source File (.c) Structure

```c
/**
 * @file    sensor_hal.c
 * @brief   Sensor HAL implementation
 */

/* ─── Includes ─────────────────────────────────────── */
#include "sensor_hal.h"
#include "main.h"           /* STM32 HAL */
#include <string.h>

/* ─── Private Constants ─────────────────────────────── */
#define ADC_VREF_MV          (3300U)
#define ADC_MAX_VALUE        (4095U)
#define TEMP_SENSOR_GAIN     (0.01f)   /* V/°C for LM35 */

/* ─── Private Variables ─────────────────────────────── */
static ADC_HandleTypeDef *s_hadc1 = NULL;

/* ─── Private Function Declarations ─────────────────── */
static float adc_to_voltage(uint16_t raw_adc);
static float voltage_to_temperature(float voltage);

/* ─── Public Function Implementations ───────────────── */
ev_status_t sensor_init(void)
{
    /* ... implementation ... */
    return EV_STATUS_OK;
}

/* ─── Private Function Implementations ──────────────── */
static float adc_to_voltage(uint16_t raw_adc)
{
    return ((float)raw_adc / ADC_MAX_VALUE) * (ADC_VREF_MV / 1000.0f);
}
```

---

## Function Rules

1. **One function = one job.** If you can't describe what it does in one sentence without "and", split it.
2. **Maximum 50 lines** per function. Longer = needs refactoring.
3. **Maximum 4 parameters.** More = use a struct.
4. **Always check return values** from HAL functions.
5. **No recursion** in embedded code (stack overflow risk).
6. **Functions that can fail must return a status code**, not just `void`.

```c
/* ✅ Good — returns status, caller can check */
ev_status_t sensor_init(void);

/* ❌ Bad — silent failure */
void sensor_init(void);
```

---

## Variable Rules

1. **Initialise every variable** before use.
2. **No global variables** unless absolutely necessary. Prefer passing via pointer.
3. **Use exact-width types** from `<stdint.h>`: `uint8_t`, `int16_t`, `uint32_t`.
4. **Never use `int` without width.** `int` size varies by platform.
5. **Use `bool`** from `<stdbool.h>` for true/false values, not `int`.

```c
/* ✅ Good */
uint8_t  throttle_pct = 0U;
float    batt_temp    = 0.0f;
bool     brake_active = false;
uint32_t tick_ms      = 0U;

/* ❌ Bad */
int throttle;
float temp;
int brake;
unsigned int tick;
```

---

## Comments & Documentation

### Rule: Comment the WHY, not the WHAT

```c
/* ❌ Bad — explains WHAT (obvious from code) */
speed = speed + 1;  /* add 1 to speed */

/* ✅ Good — explains WHY */
speed = speed + 1;  /* Compensate for encoder dead zone below 5 RPM */
```

### Every Public Function MUST Have a Doxygen Comment

```c
/**
 * @brief  Set motor speed via PWM duty cycle.
 *
 * @note   This function applies a soft-start ramp if the requested speed
 *         is more than 20% above the current speed, to prevent current surge.
 *
 * @param  speed_pct  Target speed as percentage (0 = stopped, 100 = full speed)
 * @retval EV_STATUS_OK      Speed set successfully
 * @retval EV_STATUS_ERROR   Invalid percentage or PWM hardware error
 */
ev_status_t motor_set_speed(uint8_t speed_pct);
```

### Use TODO / FIXME Tags

```c
/* TODO(member2): Replace with DMA-based ADC when HAL is ready */
/* FIXME: This formula needs calibration with real hardware     */
/* NOTE: CAN ID 0x100 is reserved for EV_STATUS frame          */
```

---

## Error Handling

### Return Type Convention

All functions that can fail use `ev_status_t`:

```c
/* In ev_types.h */
typedef enum {
    EV_STATUS_OK         = 0,
    EV_STATUS_ERROR      = 1,
    EV_STATUS_TIMEOUT    = 2,
    EV_STATUS_INVALID    = 3,
    EV_STATUS_HAL_ERROR  = 4
} ev_status_t;
```

### Never Ignore Return Values

```c
/* ❌ Bad */
sensor_init();
motor_init();

/* ✅ Good */
if (sensor_init() != EV_STATUS_OK) {
    fault_set(FAULT_SENSOR_INIT_FAILED);
    return EV_STATUS_ERROR;
}
```

### Null Pointer Checks

```c
/* ✅ Always check pointer parameters */
ev_status_t sensor_read_all(sensor_data_t *data)
{
    if (data == NULL) {
        return EV_STATUS_INVALID;
    }
    /* ... rest of function ... */
}
```

---

## Header File Guards

Every `.h` file **MUST** start and end with:

```c
#ifndef MODULE_NAME_H     /* ALL CAPS, ends in _H */
#define MODULE_NAME_H

/* ... file contents ... */

#endif /* MODULE_NAME_H */
```

---

## Magic Numbers

A "magic number" is a raw number with no name. **They are forbidden.**

```c
/* ❌ Bad — what do 60, 50, 3.0 mean? */
if (temp > 60)      { fault_set(2); }
if (current > 50)   { fault_set(4); }

/* ✅ Good — self-documenting */
#define FAULT_TEMP_CRITICAL_C    (60.0f)
#define FAULT_CURRENT_MAX_A      (50.0f)
#define FAULT_OVER_TEMP          (0x01U)
#define FAULT_OVER_CURRENT       (0x04U)

if (temp > FAULT_TEMP_CRITICAL_C)   { fault_set(FAULT_OVER_TEMP);    }
if (current > FAULT_CURRENT_MAX_A)  { fault_set(FAULT_OVER_CURRENT); }
```

> All thresholds and constants go in `ev_config.h`.

---

## Memory Usage Rules

1. **No dynamic memory allocation** (`malloc`, `calloc`, `free`) in embedded code.
   > Heap fragmentation can cause unpredictable crashes in long-running systems.
2. **No Variable Length Arrays (VLAs).**
3. **Declare large buffers as `static`** if they live for the life of the program.
4. **Stack depth:** Keep local variables small. Avoid large arrays on the stack.

```c
/* ❌ Bad — dynamic allocation */
uint8_t *buffer = malloc(256);

/* ✅ Good — static allocation */
static uint8_t s_uart_buffer[256];
```

---

## MISRA-C Inspired Rules

Simplified from MISRA-C - the gold standard in automotive software.

| Rule | Description |
|---|---|
| **MISRA-S1** | Never use `goto` |
| **MISRA-S2** | All `switch` statements must have a `default` case |
| **MISRA-S3** | Always use braces `{}` even for single-line `if` / `for` / `while` |
| **MISRA-S4** | Never use `//` comments - use `/* */` only (MISRA C90) |
| **MISRA-S5** | No implicit type conversions - always cast explicitly |
| **MISRA-S6** | Boolean expressions must be truly boolean (not `int` `0`/`1`) |

```c
/* ❌ Bad — MISRA-S3 violation */
if (fault_active)
    motor_stop();

/* ✅ Good */
if (fault_active) {
    motor_stop();
}

/* ❌ Bad — MISRA-S2 violation (missing default) */
switch (state) {
    case EV_STATE_IDLE:    break;
    case EV_STATE_RUNNING: break;
}

/* ✅ Good */
switch (state) {
    case EV_STATE_IDLE:    ev_handle_idle();    break;
    case EV_STATE_RUNNING: ev_handle_running(); break;
    default:               ev_handle_error();   break;
}
```

---

## Example Compliant Code

A complete, fully standards-compliant function for reference:

```c
/**
 * @brief  Check all sensor readings against fault thresholds.
 *
 * @param  data  Pointer to current sensor data snapshot.
 * @retval Bitmask of active fault codes (0 = no faults).
 */
uint8_t fault_check_all(const sensor_data_t *data)
{
    uint8_t faults = 0U;

    /* Null pointer guard */
    if (data == NULL) {
        return FAULT_INVALID_DATA;
    }

    /* Check battery temperature */
    if (data->batt_temp > FAULT_BATT_TEMP_CRITICAL_C) {
        faults |= FAULT_OVER_TEMP_BATT;
    }

    /* Check motor temperature */
    if (data->motor_temp > FAULT_MOTOR_TEMP_CRITICAL_C) {
        faults |= FAULT_OVER_TEMP_MOTOR;
    }

    /* Check over-current */
    if (data->current > FAULT_CURRENT_MAX_A) {
        faults |= FAULT_OVER_CURRENT;
    }

    /* Check under-voltage */
    if (data->voltage < FAULT_VOLTAGE_MIN_V) {
        faults |= FAULT_UNDER_VOLTAGE;
    }

    /* Check over-voltage */
    if (data->voltage > FAULT_VOLTAGE_MAX_V) {
        faults |= FAULT_OVER_VOLTAGE;
    }

    /* Check manual fault trigger */
    if (data->fault_switch == true) {
        faults |= FAULT_MANUAL_TRIGGER;
    }

    return faults;
}
```

### Why This Is Compliant

| Check | Detail |
|---|---|
| ✅ Doxygen comment | `@brief`, `@param`, `@retval` all present |
| ✅ `const` on pointer | Function declares it won't modify input data |
| ✅ Null pointer check | First thing in the function body |
| ✅ Named constants | No magic numbers anywhere |
| ✅ Braces on every block | All `if` statements use `{}` |
| ✅ Explicit `== true` | Boolean comparison is unambiguous |
| ✅ Returns meaningful value | Caller gets a fault bitmask, not `void` |

---

*basesync · Coding Standards · 06*
