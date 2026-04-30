# GitHub Setup Guide

| |  |
|:---|:---|
| **Organisation** | [basesync](https://github.com/basesync) |
| **Project Version** | v1.0.0 |
| **Last Updated** | 2026 |
| **Owner** | [@Rohith-Kalarikkal](https://github.com/Rohith-Kalarikkal) |
| **Status** | ✅ Approved  |

---

## Table of Contents

1. [Organisation Overview](#organisation-overview)
2. [Repository Structure](#repository-structure)
3. [Issue & PR Templates](#issue--pr-templates)
   - [Bug Report Template](#bug-report-template)
   - [Feature Request Template](#feature-request-template)
   - [Pull Request Template](#pull-request-template)
4. [Labels System](#labels-system)
5. [Milestones](#milestones)
6. [Team Permissions](#team-permissions)
7. [Daily Git Workflow](#daily-git-workflow)
8. [Commit Message Convention](#commit-message-convention)

---

## Organisation Overview

A GitHub Organisation [`basesync`](https://github.com/basesync) is like a company's GitHub account. Instead of each person having their own repo, the organisation owns the repos, and team members are added as collaborators.

---

## Repository Structure

**Main repo:** [`basesync/ev-ecu-system`](https://github.com/basesync/ev-ecu-system)

Clone the repo and **always work on a branch created from `develop`**.

```bash
git clone https://github.com/basesync/ev-ecu-system.git

# Get the develop branch locally
git checkout develop
# Make sure it's up to date
git pull origin develop

# Create your new work branch
git checkout -b feature/my-new-task
```

> If you've already cloned the repo and are currently on `main`, you can switch to `develop` and create your new feature branch in **one line**:

```bash
git fetch origin && git checkout -b feature/my-new-task origin/develop
```

---

## Issue & PR Templates

### Bug Report Template

**File:** `.github/ISSUE_TEMPLATE/bug_report.md`

```markdown
---
name: Bug Report
about: Something is not working as expected
title: '[BUG] '
labels: bug
assignees: ''
---

## Bug Description
<!-- A clear and concise description of what the bug is. -->


## Steps to Reproduce
<!-- Exact steps to reproduce the bug. Number them. -->
1.
2.
3.

## Expected Behaviour
<!-- What should have happened? -->


## Actual Behaviour
<!-- What actually happened? -->


## Environment
<!-- Fill in your setup details -->
| Field | Value |
|---|---|
| OS | <!-- Windows 11 / Ubuntu 22.04 / macOS --> |
| arm-none-eabi-gcc version | <!-- run: arm-none-eabi-gcc --version --> |
| CMake version | <!-- run: cmake --version --> |
| Testing on | <!-- Wokwi simulation / Renode / Real hardware --> |
| STM32 board (if hardware) | <!-- e.g., Blue Pill STM32F103C8T6 --> |

## UART / Serial Logs
<!-- Paste the UART output at the time of the bug. Use a code block. -->
```
paste logs here
```

## Error Output / CI Log
<!-- If this is a build or CI error, paste the relevant output -->
```
paste error here
```

## Screenshots
<!-- If applicable, add screenshots of the simulation or oscilloscope -->

## Related Requirement
<!-- Which requirement does this violate? -->
Requirement ID: <!-- e.g., FR-003-01 -->

## Possible Cause (if known)
<!-- Optional: Any idea what might be causing this? -->


## Workaround (if any)
<!-- Optional: Is there a temporary workaround? -->
```

---

### Feature Request Template

**File:** `.github/ISSUE_TEMPLATE/feature_request.md`

```markdown
---
name: User Story / Feature Request
about: New functionality to implement
title: '[S?-??] '
labels: user-story
assignees: ''
---

## User Story
<!-- Fill in the blanks:
As a [developer / system / engineer],
I want [goal],
So that [benefit / reason]. -->

As a **[role]**, I want **[goal]**, so that **[benefit]**.

## Background / Context
<!-- Why is this needed? What problem does it solve? Keep it brief. -->


## Acceptance Criteria
<!-- These are the conditions that must ALL be true for this story to be "Done".
Be specific and testable. Each criterion should be verifiable. -->
- [ ]
- [ ]
- [ ]
- [ ]

## Definition of Done
<!-- What specific, observable evidence proves this story is complete? -->
- [ ] Code committed and PR merged to develop
- [ ] Unit tests written and passing
- [ ] Cppcheck passes
- [ ] CI pipeline green
- [ ] Tested in Wokwi simulation (or hardware if applicable)
- [ ] File in [Docs](docs) folder updated (if design changed)

## Related Requirement
<!-- Link to the requirement this implements -->
Requirement ID: <!-- e.g., FR-001-01 -->

## Technical Notes / Hints
<!-- Optional: Any technical guidance, relevant STM32 peripherals, or approach suggestions -->


## Dependencies
<!-- Does this story depend on another story being done first? -->
Depends on: <!-- e.g., #12 (GPIO init must be done first) -->

## Story Points Estimate
<!-- Delete all but one -->
- [ ] 1 point — Trivial (1–2 hours)
- [ ] 2 points — Small (half a day)
- [ ] 3 points — Medium (1 day)
- [ ] 5 points — Large (2–3 days)
- [ ] 8 points — Very large (needs whole sprint)
- [ ] 13 points — Too big, must be split

## Module
<!-- Which module does this belong to? -->
- [ ] Sensor HAL
- [ ] Motor Control
- [ ] Fault Manager
- [ ] CAN Driver
- [ ] Logger
- [ ] EV State Machine
- [ ] CI/CD
- [ ] DevOps / Tooling
- [ ] Documentation
- [ ] Testing
```

---

### Pull Request Template

**File:** `.github/PULL_REQUEST_TEMPLATE.md`

```markdown
## Summary
<!-- What does this PR do? Write 2–3 sentences. Be specific. -->
<!-- Example: "Implements the battery temperature ADC read function in sensor_hal.c.
              Adds unit tests for all boundary conditions." -->


## Type of Change
<!-- Put an x inside the [ ] that applies: [x] -->
- [ ] 🆕 New feature / user story
- [ ] 🐛 Bug fix
- [ ] ♻️ Refactor (no behaviour change)
- [ ] 📝 Documentation only
- [ ] 🔧 CI/CD or tooling change
- [ ] 🔒 Security fix

## Related Issue
<!-- Every PR must be linked to a GitHub Issue -->
Closes #<!-- issue number here -->

## Changes Made
<!-- List every significant change. Be specific so the reviewer knows what to look at. -->
-
-
-

## How to Test This PR
<!-- Step-by-step instructions for the reviewer to verify this works -->
1.
2.
3.

## Testing Done by Author
<!-- Check all that apply -->
- [ ] Unit tests written for new/changed code
- [ ] All existing tests pass locally (`cd Tests/build && ./test_runner` → 0 Failures)
- [ ] Cppcheck passes locally (`cppcheck --error-exitcode=1 -I core/Inc core/src/`)
- [ ] Code compiled successfully (`cmake --build build`)
- [ ] Tested in Wokwi simulation (if applicable)
- [ ] UART output verified (if applicable)
- [ ] No new compiler warnings (`-Wall -Wextra` clean)

## Code Quality Checklist
<!-- Check all that apply. Do NOT open a PR without completing this. -->
- [ ] Code follows naming conventions from `BASESYNC-STD-001`
- [ ] All public functions have Doxygen-style comments (`@brief`, `@param`, `@retval`)
- [ ] No magic numbers — all constants are in `ev_config.h`
- [ ] All function parameters are validated (null checks where applicable)
- [ ] Return values of all called functions are checked
- [ ] All `switch` statements have a `default` case
- [ ] All `if/for/while` blocks use `{` braces even for single lines

## Documentation
- [ ] Docs folder updated if module interface or design changed
- [ ] `README.md` updated if new setup steps are needed
- [ ] Inline `/* TODO: */` comments added for any deferred work

## Screenshots / Logs (if applicable)
<!-- Paste UART output, simulation screenshots, or CI output that shows it working -->

---
<!-- Do not edit below this line -->
**Reviewer checklist:**
- [ ] Code logic is correct and matches the linked issue requirements
- [ ] Tests are meaningful (not just `assert(1 == 1)`)
- [ ] No obvious security or safety issues
- [ ] Comments are clear and accurate
```

---

## Labels System

### Module Labels

| Label | Description |
|---|---|
| `sensor-hal` | Sensor HAL module |
| `motor-ctrl` | Motor control module |
| `fault-manager` | Fault management module |
| `can-driver` | CAN bus driver |
| `ci-cd` | CI/CD pipeline related |
| `devops` | DevOps / tooling |

### Issue Type Labels

| Label | Description |
|---|---|
| `bug` | Something is broken |
| `enhancement` | Improvement to existing feature |
| `user-story` | A user story / feature |
| `documentation` | Improvements or additions to documentation |
| `question` | Further information is requested |
| `good first issue` | Good for newcomers |
| `help wanted` | Extra attention is needed |

### Status Labels

| Label | Description |
|---|---|
| `blocked` | Blocked on a dependency |
| `duplicate` | This issue or PR already exists |
| `invalid` | This doesn't seem right |
| `wontfix` | This will not be worked on |
| `dependencies` | Dependency-related |

### Sprint Labels

`sprint-1` · `sprint-2` · `sprint-3` · `sprint-4` · `sprint-5` · `sprint-6`

---

## Milestones

| Milestone | Due | Description |
|---|---|---|
| **Sprint 1 - Foundation** | Week 2 | Repo setup, sensor HAL, basic build |
| **Sprint 2 - Core Modules** | Week 4 | Motor control, fault manager, logger |
| **Sprint 3 - Communication** | Week 6 | CAN driver, state machine |
| **Sprint 4 - SIL Testing** | Week 8 | Full Wokwi simulation, Unity tests |
| **Sprint 5 - Hardware Bring-Up** | Week 10 | Flash to STM32 hardware |
| **Sprint 6 - HIL Testing** | Week 12 | BusMaster integration, HIL suite |

---

## Team Permissions

| Resource | Who Has Access | Permission Level |
|---|---|---|
| `main` branch | Lead only (via PR) | Write via PR only |
| `develop` branch | All members (via PR) | Write via PR only |
| `feature/*` branches | Each member | Full write |
| GitHub Actions | All members | Read (view logs) |
| GitHub Secrets | Lead only | Admin |
| Gitbook | Each members | Full write |

---

## Daily Git Workflow

Follow these steps **every time** you start a new task.

### Step 1 - Sync Your Local Machine

```bash
git checkout develop
git pull origin develop
```

### Step 2 - Start a New Task

> ⚠️ Never work directly on `develop`. Always create a feature branch.

```bash
git checkout -b feature/your-task-name
```

### Step 3 - Write Code, Commit Often

```bash
git add .
git commit -m "feat(sensor): add battery voltage ADC read function"
```

### Step 4 - Push Your Branch

```bash
git push origin feature/your-task-name
```

### Step 5 - Open a Pull Request on GitHub

- **Base:** `develop`
- **Compare:** `feature/your-task-name`
- Fill in the PR template completely
- Assign a reviewer

### Step 6 — After Review + CI Passes

- Merge into `develop` using **Squash and merge**
- Delete your feature branch

---

## Commit Message Convention

### Format

```
<type>(<scope>): <short description>
```

### Types

| Type | When to Use |
|---|---|
| `feat` | A new feature for the user, not a new feature for the build script. |
| `fix` | Bug fix |
| `refactor` | Code improvement, no feature change |
| `test` | Adding or updating tests |
| `docs` | Documentation only |
| `ci` | CI/CD changes |
| `test` | Adding or updating Unity unit tests or Wokwi simulation configs. |
| `chore` | Tooling, build scripts |
| `style` | Formatting, missing semi-colons, white-space (no code logic changes) |
| `perf` | A code change that improves performance. |
| `revert` | Reverts a previous commit. |

### Scopes

Our module scopes: `sensor`, `motor`, `fault`, `can`, `logger`, `sm` (state machine), `ci`, `sim`, `tests`, `build`, `repo`

### Examples

```bash
feat(sensor): add battery voltage ADC read function
fix(motor): correct PWM duty cycle scaling formula
test(fault): add over-temperature threshold unit test
docs(readme): update installation instructions
ci(actions): add Cppcheck static analysis job
```

---

*basesync · GitHub Setup · 02*
