# Environment Setup Guide

| |  |
|:---|:---|
| **Organisation** | [basesync](https://github.com/basesync) |
| **Project Version** | v1.0.0 |
| **Last Updated** | 2026 |
| **Owner** | [@Rohith-Kalarikkal](https://github.com/Rohith-Kalarikkal) |
| **Status** | 🟡 In Progress |

> ✅ **When done, you should be able to:** clone the repo, compile the firmware, run unit tests, and open the simulation.

---

## Table of Contents

1. [Prerequisites](#prerequisites)
2. [Step 1 — Install VS Code](#step-1--install-vs-code)
3. [Step 2 — Install ARM GCC Toolchain](#step-2--install-arm-gcc-toolchain)
4. [Step 3 — Install CMake & Build Tools](#step-3--install-cmake--build-tools)
5. [Step 4 — Install STM32CubeIDE](#step-4--install-stm32cubeide)
6. [Step 5 — Install VS Code Extensions](#step-5--install-vs-code-extensions)
7. [Step 6 — Install Cppcheck](#step-6--install-cppcheck)
8. [Step 7 — Install Git & Configure GitHub](#step-7--install-git--configure-github)
9. [Step 8 — Clone and Build the Project](#step-8--clone-and-build-the-project)
10. [Step 9 — Setup Wokwi Simulation](#step-9--setup-wokwi-simulation)
11. [Step 10 — Set Up BusMaster CAN Simulation](#step-10--set-up-busmaster-can-simulation)
12. [Step 11 — Run Unit Tests Locally](#step-11--run-unit-tests-locally)
13. [Verification Checklist](#verification-checklist)

---

## Prerequisites

| Requirement | Detail |
|---|---|
| **OS** | Windows 10/11, Ubuntu, or macOS |
| **RAM** | 8GB minimum (16GB recommended) |
| **Disk Space** | 10GB free |
| **Internet** | Required for downloads |
| **GitHub Account** | Must be a member of the `basesync` organisation |

---

## Step 1 — Install VS Code

VS Code is our main code editor.

1. Go to: [https://code.visualstudio.com](https://code.visualstudio.com)
2. Download the installer for your OS
3. Install with default settings
4. Launch VS Code and verify it opens

---

## Step 2 — Install ARM GCC Toolchain

This is the **compiler** — it converts your C code into binary that runs on the STM32.

### Windows

1. Go to: [Arm GNU Toolchain Downloads – Arm Developer](https://developer.arm.com/downloads/-/arm-gnu-toolchain-downloads)
2. Download: `arm-gnu-toolchain-[version]-mingw-w64-i686-arm-none-eabi.zip`
3. Right-click the `.zip` and select **Extract All...**
4. Move the extracted folder to a clean path, e.g. `C:\arm-gnu-toolchain`
5. Open the folder and navigate into the `bin` subfolder - you'll see many `.exe` files inside
6. While inside the `bin` folder, click the **Address Bar** at the top of File Explorer and **copy the path** (e.g. `C:\arm-gnu-toolchain\bin`)
7. Press the **Windows Key** and type `"env"`
8. Select **"Edit the system environment variables"** → click the **Environment Variables** button
9. In the **System variables** list (bottom), find `Path` → click **Edit** → click **New** → paste your path (`C:\arm-gnu-toolchain\bin`)
10. Click **OK** on all three windows
11. Open Command Prompt and verify:

```bash
arm-none-eabi-gcc --version
```

---

## Step 3 — Install CMake & Build Tools

CMake is our build system. It tells the compiler which files to compile and how.

### Windows

1. Download CMake: [cmake.org/download](https://cmake.org/download) (`.msi` installer)
2. During install: select **"Add CMake to system PATH"**
3. Download Make for Windows: [https://gnuwin32.sourceforge.net/packages/make.htm](https://gnuwin32.sourceforge.net/packages/make.htm)
4. Verify both tools are installed:

```bash
cmake --version
make --version
```

---

## Step 4 - Install STM32CubeIDE

STM32CubeIDE is ST Microelectronics' official IDE. We use it for:

- Pin configuration (CubeMX)
- HAL code generation
- On-chip debugging

1. Go to: [STM32CubeIDE | Software – STMicroelectronics](https://www.st.com/en/development-tools/stm32cubeide.html)
2. Create a free **MySTM32** account (required to download)
3. Download and install

> 💡 No need to fully understand CubeIDE yet. We will use its HAL generation feature, then move to VS Code for editing.

---

## Step 5 — Install VS Code Extensions

> ⚠️ **Don't install these yet** — wait until the repo is cloned so workspace settings are applied correctly.

When ready: open VS Code → press `Ctrl+Shift+X` → search and install each extension below.

| Extension | Identifier |
|---|---|
| C/C++ | `ms-vscode.cpptools` |
| CMake Tools | `ms-vscode.cmake-tools` |
| GitHub Actions | `github.vscode-github-actions` |
| Python | `ms-python.python` |
| Wokwi Simulator | `wokwi.wokwi-vscode` |
| Cppcheck | `nathanj.cppcheck-plugin` |
| Teleplot | `alexnesnes.teleplot` |
| GitLens | `eamodio.gitlens` |

---

## Step 6 — Install Cppcheck

Cppcheck analyses your C code for bugs **without running it**.

### Windows

1. Download: [https://cppcheck.sourceforge.io/](https://cppcheck.sourceforge.io/)
2. Run the installer with default settings
3. Press the **Windows Key** and type `"env"`
4. Select **"Edit the system environment variables"** → click **Environment Variables**
5. In the **System variables** list, find `Path` → click **Edit** → click **New** → paste `C:\Program Files\Cppcheck`
6. Verify:

```bash
cppcheck --version
```

### Ubuntu

```bash
sudo apt install cppcheck
cppcheck --version
```

---

## Step 7 - Install Git & Configure GitHub

### Install Git

**Windows:** [https://git-scm.com/download/win](https://git-scm.com/download/win)

**Ubuntu:**
```bash
sudo apt install git
```

**macOS:**
```bash
brew install git
```

### Configure Git

```bash
git config --global user.name "Your Full Name"
git config --global user.email "your.email@example.com"
git config --global core.autocrlf input   # Important on Windows
git config --global push.autoSetupRemote true
```

### Setup SSH Keys for GitHub

```bash
# Generate SSH key
ssh-keygen -t ed25519 -C "your.email@example.com"
# Press Enter for all prompts (default location, no passphrase for simplicity)

# Print public key (Windows, Linux, and Mac)
cat ~/.ssh/id_ed25519.pub
```

1. Copy the output
2. Go to **GitHub → Settings → SSH and GPG keys → New SSH key**
3. Paste and save
4. Test the connection:

```bash
ssh -T git@github.com
```

Expected response:
```
Hi <Username>! You've successfully authenticated.
```

### Accept Organisation Invitation

1. Check your email for an invitation from the `basesync` organisation on GitHub
2. Accept the invitation
3. You should now see `basesync` listed in your GitHub organisations

---

## Step 8 - Clone and Build the Project

```bash
# Clone the repository
git clone git@github.com:basesync/ev-ecu-system.git
cd ev-ecu-system

# Configure CMake (reads CMakeLists.txt and the ARM toolchain file)
cmake -B build -G "Unix Makefiles" -S . \
  -DCMAKE_TOOLCHAIN_FILE=cmake/arm-gcc-toolchain.cmake

# Build the firmware
cmake --build build
```

**Expected output:**
```
[100%] Linking C executable smart_ev_ecu.elf
[100%] Built target smart_ev_ecu
```

---

## Step 9 — Setup Wokwi Simulation

Wokwi lets you run STM32 code in a browser simulator — **no hardware needed**.

### Option A — Wokwi VS Code Extension *(Recommended)*

1. Install the extension: search `Wokwi` in VS Code extensions (`wokwi.wokwi-vscode`)
2. Get a free licence: [Wokwi for Visual Studio Code](https://wokwi.com/vscode) (free for open source)
3. Open `Simulation/wokwi/wokwi.toml` in VS Code
4. Press `F1` → run **Wokwi: Start Simulator**

### Option B — Wokwi Web

1. Go to: [https://wokwi.com](https://wokwi.com)
2. Create a free account
3. Import our `diagram.json`

### What You'll See

A virtual STM32 board with virtual LEDs, potentiometers, and buttons connected. Your firmware runs on it in real time.

---

## Step 10 - Set Up BusMaster CAN Simulation

BusMaster simulates a CAN bus on your PC.

1. Download: [BUSMASTER](https://rbei-etas.github.io/busmaster/)
2. Install with default settings
   > ⚠️ Windows only - use a VM on Mac/Linux
3. Open BusMaster
4. **File → Open →** `Simulation/busmaster/ev_ecu.bsm`
5. For the Virtual CAN driver, choose one of:
   - Download: [PEAK Virtual CAN driver](https://www.peak-system.com/Virtual-PEAK-Driver.305.0.html) or Vector CANalyzer trial
   - **Linux only** — use socketcan:

```bash
sudo modprobe vcan
sudo ip link add dev vcan0 type vcan
sudo ip link set up vcan0
```

---

## Step 11 - Run Unit Tests Locally

```bash
# From project root
cmake -B Tests/build -G "Unix Makefiles" -S Tests && cmake --build Tests/build
./Tests/build/test_runner
```

**Expected output:**
```
-----------------------
42 Tests, 0 Failures, 0 Ignored
OK
```

---

## Verification Checklist

> Run through this after setup. **Every item must be ✅ before you start coding.**

| Check | Command / Action | Expected Result |
|---|---|---|
| ARM GCC installed | `arm-none-eabi-gcc --version` | Shows version number |
| CMake installed | `cmake --version` | Shows `3.x.x` |
| Git installed | `git --version` | Shows version number |
| Cppcheck installed | `cppcheck --version` | Shows version number |
| SSH auth to GitHub | `ssh -T git@github.com` | `"Hi username! You've successfully authenticated"` |
| Repo clones OK | `git clone git@github.com:basesync/ev-ecu-system.git` | Clones without error |
| Firmware builds | `cmake .. && cmake --build .` | Compiles without errors |
| Tests pass | `./test_runner` | All tests pass |
| Wokwi opens | Press `F1` → Wokwi: Start Simulator | Virtual STM32 visible |
| VS Code extensions | Check Extensions panel | All 8 extensions listed as installed |

---

*basesync · Environmental Setup · 01*
