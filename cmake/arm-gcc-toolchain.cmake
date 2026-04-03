# cmake/arm-gcc-toolchain.cmake
#
# CMake Toolchain File for ARM Cortex-M cross-compilation
# Target: STM32F103C8T6 (ARM Cortex-M3)
# Compiler: arm-none-eabi-gcc
#
# Usage:
#   cmake -B build -S . -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake
#

# --- Tell CMake this is a cross-compile (not building for the host machine) -----
set(CMAKE_SYSTEM_NAME      Generic)     # No OS (bare-metal)
set(CMAKE_SYSTEM_PROCESSOR arm)         # Target architecture

# --- Specify the cross-compiler executables --------------------------------------
# CMake will search for these in your PATH
set(CMAKE_C_COMPILER       arm-none-eabi-gcc)
set(CMAKE_CXX_COMPILER     arm-none-eabi-g++)
set(CMAKE_ASM_COMPILER     arm-none-eabi-gcc)

# Utility tools (for size display and format conversion)
set(CMAKE_OBJCOPY          arm-none-eabi-objcopy)
set(CMAKE_OBJDUMP          arm-none-eabi-objdump)
set(CMAKE_SIZE             arm-none-eabi-size)

# --- CPU-specific flags ------------------------------------------------------------
# -mcpu=cortex-m3   -> Target the Cortex-M3 core in STM32F103
# -mthumb           -> Use Thumb-2 instruction set (smaller code, same performance)
# -mfloat-abi=soft  -> Software floating point (F103 has no FPU hardware)
set(CPU_FLAGS "-mcpu=cortex-m3 -mthumb -mfloat-abi=soft")

# --- C compiler flags ------------------------------------------------------------
set(CMAKE_C_FLAGS
    "${CPU_FLAGS}             \
    -std=c11                  \
    -Wall                     \
    -Wextra                   \
    -Wdouble-promotion        \
    -Wshadow                  \
    -fdata-sections           \
    -ffunction-sections       \
    -fno-common               \
    -fmessage-length=0        \
    "
    CACHE STRING "C compiler flags" FORCE
)

# Debug build: no optimisation, full debug info
set(CMAKE_C_FLAGS_DEBUG   "-Og -g3 -DDEBUG" CACHE STRING "" FORCE)

# Release build: optimise for size, strip debug info
set(CMAKE_C_FLAGS_RELEASE "-Os -DNDEBUG"    CACHE STRING "" FORCE)

# --- Linker flags --------------------------------------------------------------
# --gc-sections     -> Remove unused code/data sections (reduces binary size)
# -Map=output.map   -> Generate a linker map file for analysis
# --specs=nano.specs -> Use newlib-nano (tiny C library for embedded)
# --specs=nosys.specs -> No OS system calls (bare-metal)
set(CMAKE_EXE_LINKER_FLAGS
    "${CPU_FLAGS}                     \
    -Wl,--gc-sections                 \
    -Wl,-Map=${PROJECT_NAME}.map      \
    --specs=nano.specs                \
    --specs=nosys.specs               \
    -Wl,--print-memory-usage          \
    "
    CACHE STRING "Linker flags" FORCE
)

# --- Search path mode ---------------------------------------------------------
# Tell CMake NOT to search the host machine for libraries
# Only search the cross-compilation environment
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)

# --- Prevent CMake from adding host linker flags -----------------------------
set(CMAKE_TRY_COMPILE_TARGET_TYPE STATIC_LIBRARY)
