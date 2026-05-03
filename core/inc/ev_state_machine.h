/**
 * @file    ev_state_machine.h
 * @brief   EV ECU State Machine — state management and transitions
 *
 * @details Implements the EV operating state machine:
 *
 *          INIT → IDLE → RUNNING → FAULT → SAFE_STATE
 *                          ↑           |
 *                          └───────────┘ (reset clears fault)
 *
 *          State rules:
 *            INIT       — hardware initialising, motor locked off
 *            IDLE       — sensors running, motor command ignored (speed 0)
 *            RUNNING    — normal operation, motor follows throttle
 *            FAULT      — fault detected, motor immediately stopped
 *            SAFE_STATE — all outputs off, only reset clears this
 *
 *          Transition table:
 *            INIT       → IDLE       : init complete, no faults
 *            IDLE       → RUNNING    : throttle > deadband, no faults
 *            RUNNING    → IDLE       : throttle = 0 and brake pressed
 *            RUNNING    → FAULT      : fault_code != FAULT_NONE
 *            IDLE       → FAULT      : fault_code != FAULT_NONE
 *            FAULT      → SAFE_STATE : automatic, immediate on fault
 *            SAFE_STATE → INIT       : manual reset only (CAN command or power cycle)
 *
 *          The state machine is the ONLY code that calls motor_stop().
 *          All other fault paths must go through ev_sm_set_fault() to
 *          trigger the state machine, which then stops the motor. This
 *          ensures motor control is always centralised and auditable.
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

#ifndef EV_STATE_MACHINE_H
#define EV_STATE_MACHINE_H

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include <stdint.h>
#include <stdbool.h>
#include <stddef.h>
#include "ev_types.h"
#include "ev_config.h"

/* ─── Public Function Declarations ───────────────────────────────────────── */

/**
 * @brief  Initialise the state machine.
 *
 * @details Sets the initial state to EV_STATE_INIT and clears the active
 *          fault code. Must be called once before ev_sm_run().
 *
 *          Does NOT require any hardware handle — the state machine is
 *          a pure software module. It calls motor_stop() on transitions
 *          to FAULT/SAFE_STATE, so motor_init() must be called first.
 */
void ev_sm_init(void);

/**
 * @brief  Run one state machine cycle.
 *
 * @details Evaluates the current state, the provided sensor data, and
 *          the active fault code to determine whether a state transition
 *          is required. Performs the transition if needed.
 *
 *          This function is called every main loop iteration (10ms).
 *          It is deterministic: same inputs always produce the same
 *          output (no random or time-dependent behaviour).
 *
 *          Motor stop is called automatically when transitioning to
 *          FAULT or SAFE_STATE. The caller does not need to call
 *          motor_stop() separately.
 *
 * @param  data    Pointer to current sensor data snapshot. May be NULL
 *                 (treated as a critical fault → SAFE_STATE).
 * @param  faults  Fault bitmask from fault_check_all(). Use FAULT_NONE
 *                 if no faults were detected this cycle.
 */
void ev_sm_run(const sensor_data_t *data, fault_code_t faults);

/**
 * @brief  Get the current EV operating state.
 *
 * @retval Current state as ev_state_t enum value.
 */
ev_state_t ev_sm_get_state(void);

/**
 * @brief  Force a transition to FAULT state with a specific fault code.
 *
 * @details Used by external modules (CAN driver, watchdog) to inject faults
 *          without going through the normal sensor data path.
 *          Immediately calls motor_stop() and transitions to FAULT.
 *
 * @param  fault_code  Fault code to set. Must not be FAULT_NONE.
 */
void ev_sm_set_fault(fault_code_t fault_code);

/**
 * @brief  Attempt to reset from SAFE_STATE back to INIT.
 *
 * @details Only valid when current state is SAFE_STATE.
 *          Clears the fault code and returns to INIT state so the
 *          system re-enters the normal INIT → IDLE → RUNNING path.
 *
 *          This function is called when a reset command is received
 *          over CAN (Sprint 3) or when the operator presses a physical
 *          reset button.
 *
 * @retval EV_STATUS_OK       Reset successful, state is now INIT.
 * @retval EV_STATUS_ERROR    Cannot reset: current state is not SAFE_STATE.
 */
ev_status_t ev_sm_reset(void);

/**
 * @brief  Return a human-readable string for the current state.
 *
 * @details Used for UART diagnostic logging.
 *          Example output: "IDLE", "RUNNING", "FAULT", "SAFE_STATE"
 *
 * @retval Pointer to a constant string literal. Never NULL.
 */
const char *ev_sm_get_state_name(void);

#endif /* EV_STATE_MACHINE_H */
