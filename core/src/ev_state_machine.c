/**
 * @file    ev_state_machine.c
 * @brief   EV ECU State Machine — state transitions and output control
 *
 * @details Sprint 3 implementation.
 *
 *          State transition diagram:
 *
 *          ┌──────────┐  init OK    ┌──────────┐  throttle>0  ┌───────────┐
 *          │   INIT   │────────────▶│   IDLE   │─────────────▶│  RUNNING  │
 *          └──────────┘             └──────────┘              └───────────┘
 *                                        │ fault                 │       │
 *                                        ▼                       │ fault │
 *                                   ┌────────┐◀──────────────────┘       │
 *                                   │ FAULT  │                            │
 *                                   └────────┘                            │
 *                                        │ auto                           │ brake+
 *                                        ▼                       throttle=0│
 *                                  ┌────────────┐                         ▼
 *                                  │ SAFE_STATE │               back to  IDLE
 *                                  └────────────┘
 *                                        │ manual reset
 *                                        ▼
 *                                  back to INIT
 *
 *          Motor control rules by state:
 *            INIT       → motor locked at 0% (motor_stop() on entry)
 *            IDLE       → motor locked at 0% (motor_stop() on entry)
 *            RUNNING    → motor follows throttle
 *            FAULT      → motor locked at 0% (motor_stop() on entry)
 *            SAFE_STATE → motor locked at 0% (motor_stop() on entry)
 *
 * @author  BaseSync Team
 * @version 1.0 (Sprint 3)
 * @date    2025
 */

/* ─── Includes ────────────────────────────────────────────────────────────── */
#include "ev_state_machine.h"
#include "motor_ctrl.h"

/* ─── Module-Private State ────────────────────────────────────────────────── */

/** Current operating state — only modified by ev_sm_run() and ev_sm_set_fault() */
static ev_state_t   s_current_state  = EV_STATE_INIT;

/** Fault code set during last transition to FAULT state */
static fault_code_t s_active_faults  = FAULT_NONE;

/* ─── Private Helper ──────────────────────────────────────────────────────── */

/**
 * @brief  Execute entry actions for a new state.
 *
 * @details Called whenever a state transition occurs. Performs the
 *          mandatory actions for entering the new state.
 *
 * @param  new_state  The state being entered.
 * @param  faults     Fault code that caused the transition (FAULT_NONE if
 *                    not a fault-triggered transition).
 */
static void priv_enter_state(ev_state_t new_state, fault_code_t faults)
{
    s_current_state = new_state;
    s_active_faults = faults;

    switch (new_state)
    {
        case EV_STATE_INIT:
            /*
             * On re-entering INIT (after reset from SAFE_STATE):
             *   - Clear all fault codes
             *   - Motor stop as a precaution
             */
            s_active_faults = FAULT_NONE;
            (void)motor_stop();
            break;

        case EV_STATE_IDLE:
            /*
             * On entering IDLE:
             *   - Motor must be stopped (not just set to 0%)
             *   - motor_stop() is used, not motor_set_speed(0), because
             *     IDLE→IDLE transitions could happen if a fault clears quickly,
             *     and motor_stop() guarantees the output is 0%.
             */
            (void)motor_stop();
            break;

        case EV_STATE_RUNNING:
            /*
             * On entering RUNNING:
             *   - No immediate motor action — the main loop will call
             *     motor_set_speed(throttle_pct) on the next iteration.
             *   - We do NOT call motor_set_speed here to avoid a potentially
             *     stale throttle value from before the transition.
             */
            break;

        case EV_STATE_FAULT:
            /*
             * On entering FAULT:
             *   - Stop motor IMMEDIATELY — this is the safety-critical path.
             *   - Fault code is already stored in s_active_faults.
             *   - CAN FAULT_FRAME transmission is handled by the main loop
             *     (can_send_fault_frame()) on the next cycle.
             */
            (void)motor_stop();
            break;

        case EV_STATE_SAFE_STATE:
            /*
             * On entering SAFE_STATE:
             *   - Motor stop (belt-and-suspenders with FAULT entry stop).
             *   - All outputs are held off — only a reset clears this.
             */
            (void)motor_stop();
            break;

        default:
            /* Defensive: unknown state — stop motor and go to SAFE_STATE */
            s_current_state = EV_STATE_SAFE_STATE;
            s_active_faults = FAULT_INVALID_DATA;
            (void)motor_stop();
            break;
    }
}

/* ─── Public Function Implementations ────────────────────────────────────── */

/**
 * @brief  Initialise the state machine.
 */
void ev_sm_init(void)
{
    s_current_state = EV_STATE_INIT;
    s_active_faults = FAULT_NONE;
    /* Motor stop called by priv_enter_state via first run */
}

/**
 * @brief  Run one state machine iteration.
 */
void ev_sm_run(const sensor_data_t *data, fault_code_t faults)
{
    /*
     * NULL data guard — treat as critical fault immediately.
     * A NULL pointer means sensor_read_all() failed, which is a hardware
     * or software bug that cannot be safely recovered from at runtime.
     */
    if (data == NULL)
    {
        priv_enter_state(EV_STATE_SAFE_STATE, FAULT_INVALID_DATA);
        return;
    }

    /*
     * Any non-zero fault code forces transition to FAULT state
     * regardless of current state. This check runs first so a fault
     * detected while in RUNNING bypasses the normal running logic.
     */
    if (faults != FAULT_NONE)
    {
        if ((s_current_state != EV_STATE_FAULT) &&
            (s_current_state != EV_STATE_SAFE_STATE))
        {
            priv_enter_state(EV_STATE_FAULT, faults);
        }
        else if (s_current_state == EV_STATE_FAULT)
        {
            /*
             * Already in FAULT — transition immediately to SAFE_STATE.
             * Motor was already stopped on FAULT entry; motor_stop() is
             * called again in priv_enter_state(SAFE_STATE) as a redundant
             * safety measure.
             */
            priv_enter_state(EV_STATE_SAFE_STATE, faults);
        }
        /* If already in SAFE_STATE, stay there — no automatic exit */
        return;
    }

    /* No faults — evaluate normal state transitions */
    switch (s_current_state)
    {
        case EV_STATE_INIT:
            /*
             * INIT → IDLE transition: hardware initialisation is complete.
             * The main loop calls ev_sm_run() only after sensor_init() and
             * motor_init() have both returned EV_STATUS_OK, so by the time
             * we are here the hardware is ready.
             */
            priv_enter_state(EV_STATE_IDLE, FAULT_NONE);
            break;

        case EV_STATE_IDLE:
            /*
             * IDLE → RUNNING transition:
             * Throttle above deadband (handled inside motor_set_speed)
             * and brake not pressed.
             */
            if ((data->throttle_pct > EV_THROTTLE_DEADBAND_PCT) &&
                (data->brake_active == false))
            {
                priv_enter_state(EV_STATE_RUNNING, FAULT_NONE);
            }
            break;

        case EV_STATE_RUNNING:
            /*
             * RUNNING → IDLE transition:
             * Throttle at zero AND brake pressed.
             * Only brake + zero throttle together cause this transition —
             * brake alone at speed just holds motor_stop() via motor_control
             * in main.c, it does not change the state.
             */
            if ((data->throttle_pct == 0U) && (data->brake_active == true))
            {
                priv_enter_state(EV_STATE_IDLE, FAULT_NONE);
            }
            break;

        case EV_STATE_FAULT:
            /*
             * FAULT → SAFE_STATE transition is automatic and immediate.
             * We transition here on the NEXT cycle after entering FAULT.
             * (FAULT entry is handled in the fault-code check above.)
             */
            priv_enter_state(EV_STATE_SAFE_STATE, s_active_faults);
            break;

        case EV_STATE_SAFE_STATE:
            /* Stay in SAFE_STATE until ev_sm_reset() is explicitly called. */
            break;

        default:
            /* Defensive: unknown state — go to SAFE_STATE */
            priv_enter_state(EV_STATE_SAFE_STATE, FAULT_INVALID_DATA);
            break;
    }
}

/**
 * @brief  Get the current EV operating state.
 */
ev_state_t ev_sm_get_state(void)
{
    return s_current_state;
}

/**
 * @brief  Externally force a FAULT state transition.
 */
void ev_sm_set_fault(fault_code_t fault_code)
{
    if (fault_code == FAULT_NONE)
    {
        /* Ignore FAULT_NONE — it would be a no-op */
        return;
    }

    priv_enter_state(EV_STATE_FAULT, fault_code);
}

/**
 * @brief  Reset from SAFE_STATE to INIT.
 */
ev_status_t ev_sm_reset(void)
{
    if (s_current_state != EV_STATE_SAFE_STATE)
    {
        return EV_STATUS_ERROR;
    }

    priv_enter_state(EV_STATE_INIT, FAULT_NONE);
    return EV_STATUS_OK;
}

/**
 * @brief  Return human-readable state name.
 */
const char *ev_sm_get_state_name(void)
{
    switch (s_current_state)
    {
        case EV_STATE_INIT:       return "INIT";
        case EV_STATE_IDLE:       return "IDLE";
        case EV_STATE_RUNNING:    return "RUNNING";
        case EV_STATE_FAULT:      return "FAULT";
        case EV_STATE_SAFE_STATE: return "SAFE_STATE";
        default:                  return "UNKNOWN";
    }
}
