## Summary
<!-- What does this PR do? Write 2–3 sentences. Be specific. -->
<!-- Example: "Implements the battery temperature ADC read function in sensor_hal.c.
              Adds unit tests for all boundary conditions. Updates the pin mapping table in Confluence." -->


## Type of Change
<!-- Put an x inside the [ ] that applies: [x] -->
- [ ] 🆕 New feature / user story
- [ ] 🐛 Bug fix
- [ ] ♻️  Refactor (no behaviour change)
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
- [ ] Code follows naming conventions from `06_CODING_STANDARDS.md`
- [ ] All public functions have Doxygen-style comments (`@brief`, `@param`, `@retval`)
- [ ] No magic numbers — all constants are in `ev_config.h`
- [ ] All function parameters are validated (null checks where applicable)
- [ ] Return values of all called functions are checked
- [ ] All `switch` statements have a `default` case
- [ ] All `if/for/while` blocks use `{` braces even for single lines

## Documentation
- [ ] Confluence updated if module interface or design changed
- [ ] `README.md` updated if new setup steps are needed
- [ ] Inline `/* TODO: */` comments added for any deferred work

## Screenshots / Logs (if applicable)
<!-- Paste UART output, simulation screenshots, or CI output that shows it working -->

---
<!-- Do not edit below this line -->
**Reviewer checklist:**
- [ ] Code logic is correct and matches the linked issue requirements
- [ ] Tests are meaningful (not just "assert(1 == 1)")
- [ ] No obvious security or safety issues
- [ ] Comments are clear and accurate
