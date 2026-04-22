# CMakeLists.txt - EV ECU Root Build Configuration
# -----------------------------------------------------------------------------
# This is the MAIN build file. CMake reads this first.
# It describes: what to compile, how to compile it, and what to link together.
#
# Usage:
#   mkdir build && cd build
#   cmake .. -DCMAKE_TOOLCHAIN_FILE=../cmake/arm-gcc-toolchain.cmake
#   cmake --build .
# -----------------------------------------------------------------------------

cmake_minimum_required(VERSION 3.20)

# --- Project Definition --------------------------------------------------------
project(ev-ecu-system
    VERSION     0.1.0
    DESCRIPTION "EV Control and Diagnostics System — STM32 ECU Firmware"
    LANGUAGES   C ASM
)

set(CMAKE_EXECUTABLE_SUFFIX ".elf")

# --- C Standard --------------------------------------------------------------
set(CMAKE_C_STANDARD          11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS        OFF)   # Use strict C11, no GNU extensions

# --- Build type default ------------------------------------------------------
if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug CACHE STRING "Build type" FORCE)
endif()

message(STATUS "Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS "Toolchain:  ${CMAKE_C_COMPILER}")

# --- Source Files -------------------------------------------------------------
# List every .c file in the project
# Note: We list them explicitly (not glob) so CMake detects new files properly
set(FIRMWARE_SOURCES
    core/src/main.c
    # --- Sprint 2 onwards: add module source files here ---
    # core/src/sensor_hal.c
    # core/src/motor_ctrl.c
    # core/src/fault_manager.c
    # core/src/can_driver.c
    # core/src/logger.c
    # core/src/ev_state_machine.c
)

# --- Header Include Directories -------------------------------------------
set(FIRMWARE_INCLUDES
    core/inc
    drivers/STM32_HAL
)

# --- Linker Script ------------------------------------------------------------
# The linker script tells the linker where to put code and data in memory.
# For now, use a generic Cortex-M3 linker script.
# Replace with STM32F103-specific script when HAL is added.
set(LINKER_SCRIPT "${CMAKE_SOURCE_DIR}/cmake/STM32F103C8TX_FLASH.ld")

# --- Create the firmware executable -------------------------------------------
add_executable(${PROJECT_NAME} ${FIRMWARE_SOURCES})

# Apply include paths
target_include_directories(${PROJECT_NAME} PRIVATE ${FIRMWARE_INCLUDES})

# Apply linker script (only if it exists - allows building without it for CI checks)
if(EXISTS ${LINKER_SCRIPT})
    target_link_options(${PROJECT_NAME} PRIVATE
        -T${LINKER_SCRIPT}
    )
endif()

# --- Post-build: Generate .bin and .hex files ---------------------------------
# .bin = raw binary (used for flashing with STM32CubeProgrammer)
# .hex = Intel HEX format (used with some bootloaders)
add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
    COMMAND ${CMAKE_OBJCOPY} -O binary
            $<TARGET_FILE:${PROJECT_NAME}>
            ${PROJECT_NAME}.bin
    COMMAND ${CMAKE_OBJCOPY} -O ihex
            $<TARGET_FILE:${PROJECT_NAME}>
            ${PROJECT_NAME}.hex
    COMMAND ${CMAKE_SIZE} --format=berkeley $<TARGET_FILE:${PROJECT_NAME}>
    COMMENT "Generating .bin and .hex, displaying size..."
)

# --- Print configuration summary -------------------------------------------
message(STATUS "-------------------------------------------")
message(STATUS " EV ECU Build Configuration")
message(STATUS "-------------------------------------------")
message(STATUS " Project:    ${PROJECT_NAME} v${PROJECT_VERSION}")
message(STATUS " Compiler:   ${CMAKE_C_COMPILER}")
message(STATUS " Build type: ${CMAKE_BUILD_TYPE}")
message(STATUS " C Standard: C${CMAKE_C_STANDARD}")
message(STATUS "-------------------------------------------")
