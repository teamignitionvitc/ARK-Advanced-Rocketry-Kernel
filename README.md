# ARK: Advanced Rocketry Kernel

![ARK Logo](Documentation/public/ark_logo.png)

**ARK** is a robust, multi-architecture avionics framework designed for reliability and portability. It supports **ESP32**, **RP2040 (Pico)**, and **PC Simulation (SIL)** out of the box.

## Features

-   **Multi-Core Architecture**: "Superior Loop" ensuring critical timing.
-   **HAL Abstraction**: Write once, run on any supported hardware.
-   **Micro-Kernel Design**: Separate kernels for System, Modules, and User logic.
-   **FDIR**: Built-in Fault Detection, Isolation, and Recovery.
-   **Shadow Registry**: State persistence across reboots.

## Quick Start

### 1. Build for Simulation (SIL)

```bash
# Build
./Tools/build.sh -t SIL

# Run
./Build/ARK_Avionics_SIL
```

### 2. Build for Hardware

See [Documentation/GettingStarted.md](Documentation/GettingStarted.md) for detailed hardware setup.

## Documentation

-   [Getting Started](Documentation/GettingStarted.md)
-   [Architecture Overview](Documentation/02-architecture.md)
-   [Contribution Guide](Documentation/Meta/CONTRIBUTING.md)

## License

See [LICENSE](Documentation/Meta/LICENSE).
