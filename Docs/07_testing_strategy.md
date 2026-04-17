# Testing Strategy — EV ECU

| Field | Value |
|---|---|
| **Document ID** | BASESYNC-TEST-001 |
| **Version** | 1.0 |

---

## Table of Contents

1. [Testing Philosophy](#testing-philosophy)
2. [Unit Testing with Unity Framework](#unit-testing-with-unity-framework)
   - [What Is Unity?](#what-is-unity)
   - [How Unity Works](#how-unity-works)
   - [Unity Assertion Reference](#unity-assertion-reference)
   - [Test Naming Convention](#test-naming-convention)
3. [Test Plan by Module](#test-plan-by-module)
   - [sensor_hal](#module-sensor_hal)
   - [motor_ctrl](#module-motor_ctrl)
   - [fault_manager](#module-fault_manager)
   - [can_driver](#module-can_driver)
   - [ev_state_machine](#module-ev_state_machine)
4. [Mock / Stub Strategy](#mock--stub-strategy)
5. [SIL (Software-In-the-Loop) Testing](#sil-software-in-the-loop-testing)
6. [HIL (Hardware-In-the-Loop) Testing](#hil-hardware-in-the-loop-testing)
7. [Test Coverage Targets](#test-coverage-targets)

---

## Testing Philosophy

> *"Test early, test often, test automatically."*

```
                    ┌─────────────────┐
                    │   HIL Tests     │  ← Few tests, expensive, real hardware
                    │                 │    (Sprint 6)
                  ┌─┴─────────────────┴─┐
                  │  SIL / Integ Tests  │  ← Wokwi simulation tests
                  │                     │    (Sprint 4)
                ┌─┴─────────────────────┴─┐
                │      Unit Tests         │  ← Many tests, fast, no hardware
                │   (Unity Framework)     │    (Every sprint)
                └─────────────────────────┘
```

---

## Unit Testing with Unity Framework

### What Is Unity?

Unity is a C unit testing framework used by embedded engineers worldwide. It is:

- Written in pure C (no C++ needed)
- Tiny (fits on any MCU)
- Used in MISRA-compliant projects
- Easy to integrate with CMake and CI

---

### How Unity Works

```c
/* test_fault_manager.c */
#include "unity.h"
#include "fault_manager.h"

void setUp(void) {
    /* Runs before EVERY test */
    fault_manager_init();
}

void tearDown(void) {
    /* Runs after EVERY test */
    fault_clear_all();
}

/* TEST: over-temperature fault sets correct bit */
void test_fault_over_temp_sets_batt_fault_bit(void)
{
    sensor_data_t data = {0};
    data.batt_temp = 65.0f;  /* Above 60°C threshold */

    uint8_t faults = fault_check_all(&data);

    TEST_ASSERT_BIT_HIGH(0, faults);  /* Bit 0 = FAULT_OVER_TEMP_BATT */
}

/* TEST: normal temperature should NOT set fault */
void test_fault_normal_temp_no_fault(void)
{
    sensor_data_t data = {0};
    data.batt_temp = 35.0f;  /* Normal temperature */

    uint8_t faults = fault_check_all(&data);

    TEST_ASSERT_BIT_LOW(0, faults);
}

/* TEST: null pointer returns FAULT_INVALID_DATA */
void test_fault_check_null_pointer_returns_invalid(void)
{
    uint8_t result = fault_check_all(NULL);
    TEST_ASSERT_EQUAL(FAULT_INVALID_DATA, result);
}
```

---

### Unity Assertion Reference

| Assertion | What It Checks |
|---|---|
| `TEST_ASSERT_EQUAL(expected, actual)` | Two values are equal |
| `TEST_ASSERT_NOT_EQUAL(a, b)` | Two values are not equal |
| `TEST_ASSERT_TRUE(condition)` | Condition is true |
| `TEST_ASSERT_FALSE(condition)` | Condition is false |
| `TEST_ASSERT_NULL(pointer)` | Pointer is `NULL` |
| `TEST_ASSERT_NOT_NULL(pointer)` | Pointer is not `NULL` |
| `TEST_ASSERT_FLOAT_WITHIN(delta, expected, actual)` | Float within tolerance |
| `TEST_ASSERT_BIT_HIGH(bit, value)` | Specific bit is set |
| `TEST_ASSERT_BIT_LOW(bit, value)` | Specific bit is clear |
| `TEST_ASSERT_EQUAL_STRING(a, b)` | Strings match |

---

### Test Naming Convention

Pattern: `test_<module>_<condition>_<expected_result>()`

```c
test_fault_over_temp_sets_batt_fault_bit()
test_fault_normal_temp_no_fault()
test_motor_brake_active_sets_pwm_zero()
test_sensor_null_pointer_returns_invalid()
test_can_fault_frame_has_correct_id()
```

---

## Test Plan by Module

### Module: `sensor_hal`

| Test ID | Condition | Expected |
|---|---|---|
| TC-SENSOR-001 | Battery temp ADC at 0% (0V) | Returns ~0°C |
| TC-SENSOR-002 | Battery temp ADC at 50% (1.65V) | Returns ~50°C (LM35 model) |
| TC-SENSOR-003 | Battery temp ADC at 100% (3.3V) | Returns ~100°C |
| TC-SENSOR-004 | Current sense at mid-rail (2.5V) | Returns 0A (ACS712 null) |
| TC-SENSOR-005 | Current sense at 3.0V | Returns positive current |
| TC-SENSOR-006 | Throttle at 0V | Returns 0% |
| TC-SENSOR-007 | Throttle at 3.3V | Returns 100% |
| TC-SENSOR-008 | Throttle at 1.65V | Returns 50% ±2% |
| TC-SENSOR-009 | Brake GPIO HIGH | Returns `true` |
| TC-SENSOR-010 | Brake GPIO LOW | Returns `false` |
| TC-SENSOR-011 | `sensor_read_all(NULL)` | Returns `EV_STATUS_INVALID` |

---

### Module: `motor_ctrl`

| Test ID | Condition | Expected |
|---|---|---|
| TC-MOTOR-001 | `motor_set_speed(0)` | PWM duty = 0% |
| TC-MOTOR-002 | `motor_set_speed(100)` | PWM duty = 100% |
| TC-MOTOR-003 | `motor_set_speed(50)` | PWM duty = 50% ±1% |
| TC-MOTOR-004 | `motor_stop()` from running | PWM = 0 immediately |
| TC-MOTOR-005 | `motor_set_speed(150)` — invalid | Returns `EV_STATUS_INVALID` |
| TC-MOTOR-006 | Brake active during `motor_set_speed(80)` | Speed remains 0 |

---

### Module: `fault_manager`

| Test ID | Condition | Expected |
|---|---|---|
| TC-FAULT-001 | `batt_temp = 60.1°C` | `FAULT_OVER_TEMP_BATT` set |
| TC-FAULT-002 | `batt_temp = 59.9°C` | `FAULT_OVER_TEMP_BATT` NOT set |
| TC-FAULT-003 | `batt_temp = 60.0°C` (boundary) | `FAULT_OVER_TEMP_BATT` NOT set |
| TC-FAULT-004 | `motor_temp = 80.1°C` | `FAULT_OVER_TEMP_MOTOR` set |
| TC-FAULT-005 | `current = 50.1A` | `FAULT_OVER_CURRENT` set |
| TC-FAULT-006 | `voltage = 2.99V` | `FAULT_UNDER_VOLTAGE` set |
| TC-FAULT-007 | `voltage = 4.21V` | `FAULT_OVER_VOLTAGE` set |
| TC-FAULT-008 | `fault_switch = true` | `FAULT_MANUAL_TRIGGER` set |
| TC-FAULT-009 | Multiple faults simultaneously | All fault bits set correctly |
| TC-FAULT-010 | `fault_check_all(NULL)` | Returns `FAULT_INVALID_DATA` |
| TC-FAULT-011 | `fault_is_critical()` with critical fault | Returns `true` |
| TC-FAULT-012 | `fault_is_critical()` with no fault | Returns `false` |

---

### Module: `can_driver`

| Test ID | Condition | Expected |
|---|---|---|
| TC-CAN-001 | `can_send_status()` | CAN frame ID = `0x100` |
| TC-CAN-002 | Status frame byte 0 | Contains current EV state |
| TC-CAN-003 | `can_send_fault_frame(0x01)` | CAN frame ID = `0x1FF` |
| TC-CAN-004 | Fault frame byte 0 | Contains fault code |
| TC-CAN-005 | `can_send_sensor_pack1()` | CAN frame ID = `0x101` |

---

### Module: `ev_state_machine`

| Test ID | Condition | Expected |
|---|---|---|
| TC-SM-001 | After init with all OK | State = `EV_STATE_IDLE` |
| TC-SM-002 | `IDLE` + throttle = 10% | State transitions to `RUNNING` |
| TC-SM-003 | `IDLE` + throttle = 0% | State stays `IDLE` |
| TC-SM-004 | `RUNNING` + fault triggered | State transitions to `FAULT` |
| TC-SM-005 | `FAULT` state | Motor PWM = 0 |
| TC-SM-006 | `FAULT` + clear command | State transitions to `SAFE` |
| TC-SM-007 | `SAFE` + reset | State transitions to `IDLE` |

---

## Mock / Stub Strategy

Unit tests must run on a **PC**, not on STM32 hardware. This means we need stubs — fake versions of the HAL functions.

```
Tests/
└── mocks/
    ├── mock_stm32_hal_adc.c    ← Fake ADC that returns test values
    ├── mock_stm32_hal_gpio.c   ← Fake GPIO
    ├── mock_stm32_hal_tim.c    ← Fake PWM timer
    └── mock_stm32_hal_uart.c   ← Fake UART (capture output)
```

### Example Mock

```c
/* mock_stm32_hal_adc.c */

static uint16_t s_mock_adc_values[5] = {0};

/* Test helper: set what the ADC "returns" */
void mock_adc_set_value(uint8_t channel, uint16_t value) {
    s_mock_adc_values[channel] = value;
}

/* The real HAL function signature, but fake implementation */
HAL_StatusTypeDef HAL_ADC_PollForConversion(ADC_HandleTypeDef *hadc, uint32_t Timeout) {
    (void)hadc; (void)Timeout;
    return HAL_OK;
}

uint32_t HAL_ADC_GetValue(ADC_HandleTypeDef *hadc) {
    (void)hadc;
    return s_mock_adc_values[0]; /* simplified */
}
```

---

## SIL (Software-In-the-Loop) Testing

SIL means running the complete firmware in a **simulator**, not on real hardware.

**Tool:** [Wokwi](https://wokwi.com)

### What Wokwi Simulates

- STM32F103 microcontroller
- ADC inputs (via virtual potentiometers)
- GPIO inputs (via virtual buttons)
- UART output (visible in terminal panel)
- PWM output (visible as LED brightness)

### SIL Test Procedure

1. Open Wokwi with `diagram.json`
2. Start simulation
3. Check UART terminal shows Teleplot data
4. Turn virtual potentiometer → verify motor PWM changes
5. Press virtual brake button → verify motor stops
6. Press virtual fault button → verify state machine enters `FAULT`
7. Check CAN output (via UART-CAN bridge simulation)

### Wokwi `diagram.json` Skeleton

```json
{
  "version": 1,
  "author": "BaseSync Team",
  "editor": "wokwi",
  "parts": [
    { "type": "board-st-nucleo-f103rb", "id": "cpu",       "top": 0,   "left": 0   },
    { "type": "wokwi-potentiometer",    "id": "throttle",  "top": 100, "left": 300,
      "attrs": { "label": "Throttle" } },
    { "type": "wokwi-potentiometer",    "id": "batt_temp", "top": 180, "left": 300,
      "attrs": { "label": "Battery Temp" } },
    { "type": "wokwi-pushbutton",       "id": "brake_btn", "top": 260, "left": 300 },
    { "type": "wokwi-pushbutton",       "id": "fault_btn", "top": 310, "left": 300 },
    { "type": "wokwi-led",              "id": "motor_led", "top": 100, "left": 450,
      "attrs": { "color": "green", "label": "Motor PWM" } },
    { "type": "wokwi-led",              "id": "fault_led", "top": 180, "left": 450,
      "attrs": { "color": "red",   "label": "Fault"     } }
  ],
  "connections": [
    ["cpu:PA0",  "throttle:SIG",  "green",  []],
    ["cpu:PA1",  "batt_temp:SIG", "orange", []],
    ["cpu:PB0",  "brake_btn:1.l", "blue",   []],
    ["cpu:PB1",  "fault_btn:1.l", "yellow", []],
    ["cpu:PA8",  "motor_led:A",   "green",  []],
    ["cpu:PC13", "fault_led:A",   "red",    []]
  ]
}
```

---

## HIL (Hardware-In-the-Loop) Testing

HIL = **Real STM32 hardware**, but sensors and motor replaced by a PC-controlled simulator.

**Tool:** [BusMaster](https://rbei-etas.github.io/busmaster/)

### What BusMaster Does

- Monitors all CAN frames transmitted by the STM32
- Injects CAN frames (simulate commands)
- Logs all frames with timestamps
- Runs CAPL-like test scripts

### HIL Test Configuration

```
PC running BusMaster
        │
        │  USB-CAN adapter (PEAK PCAN-USB)
        │
   CAN Bus  (120Ω termination at each end)
        │
   STM32 + TJA1050 CAN transceiver
```

### BusMaster Test Script (Pseudo-code)

```c
// HIL Test: Over-temperature fault injection
TEST "HIL_FAULT_OVER_TEMP" {

    // Step 1: Wait for status frame showing RUNNING state
    WAIT_FOR_CAN_FRAME(0x100, timeout=2000ms)
    ASSERT frame.byte[0] == EV_STATE_RUNNING

    // Step 2: Use test GPIO to inject over-temp condition
    SET_GPIO(TEST_INJECT_OVER_TEMP, HIGH)

    // Step 3: Expect FAULT_FRAME within 10ms (NFR-001)
    WAIT_FOR_CAN_FRAME(0x1FF, timeout=10ms)
    ASSERT frame.byte[0] & FAULT_OVER_TEMP_BATT != 0

    // Step 4: Expect STATUS_FRAME showing FAULT state
    WAIT_FOR_CAN_FRAME(0x100, timeout=100ms)
    ASSERT frame.byte[0] == EV_STATE_FAULT

    PASS "Over-temperature fault correctly detected and reported"
}
```

---

## Test Coverage Targets

| Module | Target Coverage | Measurement Tool |
|---|---|---|
| `sensor_hal` | ≥ 85% | gcov (run in CI) |
| `motor_ctrl` | ≥ 85% | gcov |
| `fault_manager` | ≥ 90% | gcov |
| `can_driver` | ≥ 80% | gcov |
| `ev_state_machine` | ≥ 90% | gcov |
| `logger` | ≥ 75% | gcov |
| **Overall** | **≥ 80%** | gcov + Codecov |

### Adding Coverage to CI

Add to `unit-tests.yml`:

```yaml
- name: Run tests with coverage
  run: |
    cd Tests/build
    cmake .. -DCOVERAGE=ON
    make
    ./test_runner
    gcov --all-blocks ../Core/Src/*.c

- name: Upload to Codecov
  uses: codecov/codecov-action@v3
  with:
    token: ${{ secrets.CODECOV_TOKEN }}
```

---

*BASESYNC-TEST-001 · v1.0*
