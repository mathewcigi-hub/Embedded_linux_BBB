# 4-Digit Counter on BeagleBone Black

This project demonstrates how to implement a **4-digit 7-segment counter** on the **BeagleBone Black (BBB)**.  
The counter can operate in **up**, **down**, or **random** counting modes.

---

## Features
- Runs on BeagleBone Black with Debian (from eMMC).
- Cross-compiled on Windows using **Eclipse IDE**.
- Executable transferred to BBB via **Remote System Explorer** (SFTP).
- Supports modes:
  - `up` (incrementing counter)
  - `down` (decrementing counter)
  - `random` (random numbers on display)

---

## Setup Instructions

### 1. BeagleBone Black Setup
- Ensure BBB boots from **eMMC** with Debian.
- Connect BBB to PC using a USB cable.
- Use **Tera Term** (or PuTTY) to log in to the BBB.

### 2. Development Environment (Windows)
1. Install **Eclipse IDE**.
2. Install the **cross-compilation toolchain** for ARM (e.g., `arm-linux-gnueabihf-gcc`).
3. Add `make` utility for Windows.
4. In Eclipse:
   - **New → C Project → Cross GCC**.
   - Configure environment variable `PATH` to point to toolchain.
   - Set **Cross Compiler prefix** and toolchain path.
   - Build project → generates ARM binary.

### 3. Remote System Explorer (RSE)
1. Install **Remote System Explorer** plugin in Eclipse.
2. Add BBB as a remote system:
   - Connection type: SSH
   - Access SFTP files.
3. File Structure:
   >Local
      > Local files
      > Local shells
      BBB
      > Sftp files
      > My home
        > bin
        > Desktop
      > Root
      > Ssh Shells
