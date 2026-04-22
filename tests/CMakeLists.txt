# Tests/CMakeLists.txt — Unit Test Build Configuration
# ---------------------------------------------------------------------------
# IMPORTANT: This builds with NATIVE GCC (your host machine's compiler),
# NOT with the ARM cross-compiler.
#
# Why? Unit tests run on the CI server (x86 Linux), not on the STM32.
# We use stub/mock functions to replace the STM32 HAL calls.
#
# Usage:
#   cd Tests
#   mkdir build && cd build
#   cmake ..         <- No toolchain file needed! Uses native GCC.
#   make
#   ./test_runner
# ---------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

project(ev_ecu_tests
    VERSION     0.1.0
    DESCRIPTION "EV ECU Unit Tests - Unity Framework"
    LANGUAGES   C
)

# --- C Standard ---------------------------------------------------------------
set(CMAKE_C_STANDARD          11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)

# --- Coverage Option ---------------------------------------------------------
option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

# --- Compiler flags for tests -------------------------------------------------
set(TEST_COMPILE_FLAGS
    -Wall
    -Wextra
    -Wno-unused-parameter    # HAL stubs often have unused params
    -g
    -O0                      # No optimisation - we want accurate coverage
    -pipe                    # Forces memory-to-memory communication
    -fno-use-linker-plugin   # Prevents LTO from writing to C:\WINDOWS
    -fno-lto                 # Explicitly disable Link Time Optimization for tests
)

if(ENABLE_COVERAGE)
    list(APPEND TEST_COMPILE_FLAGS --coverage -fprofile-arcs -ftest-coverage)
endif()

# --- Unity Framework Source -----------------------------------------------------
set(UNITY_SOURCES
    Unity/unity.c
)

# --- Mock / Stub Sources -------------------------------------------------------
# These are fake versions of STM32 HAL functions.
# They let unit tests run on a PC without real hardware.
set(MOCK_SOURCES
    # mocks/mock_stm32_hal_adc.c   - Add in Sprint 2
    # mocks/mock_stm32_hal_gpio.c  - Add in Sprint 2
    # mocks/mock_stm32_hal_tim.c   - Add in Sprint 2
    # mocks/mock_stm32_hal_uart.c  - Add in Sprint 2
)

# --- Firmware Sources Under Test --------------------------------------------
# Add the actual firmware .c files you want to test.
# Do NOT add main.c here — it has the main() function which conflicts.
set(FIRMWARE_SOURCES_UNDER_TEST
    # ../core/src/sensor_hal.c     - Add in Sprint 2
    # ../core/src/motor_ctrl.c     - Add in Sprint 3
    # ../core/src/fault_manager.c  - Add in Sprint 3
    # ../core/src/can_driver.c     - Add in Sprint 3
    # ../core/src/logger.c         - Add in Sprint 4
    # ../core/src/ev_state_machine.c - Add in Sprint 4
)

# --- Test Files --------------------------------------------------------------
set(TEST_SOURCES
    test_placeholder.c
    # test_sensor_hal.c     - Add in Sprint 2
    # test_motor_ctrl.c     - Add in Sprint 3
    # test_fault_manager.c  - Add in Sprint 3
    # test_can_driver.c     - Add in Sprint 3
    # test_ev_state_machine.c - Add in Sprint 4
    test_runner.c
)

# --- Include Directories -----------------------------------------------------
set(TEST_INCLUDES
    Unity/                  # Unity headers
    ../core/inc             # Firmware headers
    mocks/                  # Mock/stub headers
)

# --- Create the test runner executable ---------------------------------------
add_executable(test_runner
    ${UNITY_SOURCES}
    ${MOCK_SOURCES}
    ${FIRMWARE_SOURCES_UNDER_TEST}
    ${TEST_SOURCES}
)

target_include_directories(test_runner PRIVATE ${TEST_INCLUDES})
target_compile_options(test_runner PRIVATE ${TEST_COMPILE_FLAGS})

if(ENABLE_COVERAGE)
    target_link_options(test_runner PRIVATE --coverage)
endif()

# --- Enable CTest integration --------------------------------------------
enable_testing()
add_test(NAME UnitTests COMMAND test_runner)
