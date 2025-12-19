<div align="center">

<img src="public/ARK.jpeg" alt="ARK Logo" width="420"/>

<h1>ARK</h1>

<h3>Advanced Rocketry Kernel</h3>

<p><em>Professional Flight Computer Firmware for Model Rocketry</em></p>

<p>
  <img src="https://img.shields.io/badge/language-C%2B%2B-blue" alt="Language"/>
  <img src="https://img.shields.io/badge/domain-Flight%20Computing-lightgrey" alt="Domain"/>
  <img src="https://img.shields.io/badge/focus-Safety%20Critical-red" alt="Focus"/>
  <img src="https://img.shields.io/badge/status-In%20Development-yellow" alt="Status"/>
</p>

<p>
  <a href="#what-is-ark">What is ARK?</a> •
  <a href="#design-philosophy">Design Philosophy</a> •
  <a href="#architecture-overview">Architecture</a> •
  <a href="#project-structure">Project Structure</a> •
  <a href="#usage-concept">Usage Concept</a>
</p>

<hr/>

<img src="https://camo.githubusercontent.com/fbe262a7085aeff41e78915220b88792c6cd26c1964e2c4b91e312e948f2d7e8/68747470733a2f2f7465616d69676e6974696f6e2e73706163652f6c6f676f2f686561642d6c6f676f2e706e67" alt="Team Ignition Logo" width="140"/>

<p>
<strong>Developed by Team Ignition</strong><br/>
<em>Vellore Institute of Technology, Chennai</em>
</p>

</div>

---

## What is ARK?

**ARK (Advanced Rocketry Kernel)** is a **robust, safe, and heavily tested SDK** designed to support the development of a **Flight Operating System**.

ARK is **not an application** and **not a monolithic firmware**.  
It is a **restricted execution kernel** that exposes only flight-safe interfaces required for:

- Sensor data acquisition and filtering  
- Flight state estimation inputs  
- Deterministic execution control  
- Payload isolation  

The **Flight Operating System (Flight OS)** built on top of ARK is responsible for scheduling, supervision, and orchestration.

> **Flight safety always takes precedence over payload execution.**

---

## Core Principle


ARK enforces this principle by design:

- Flight logic always has the highest priority
- Payload applications run only during free cycles
- Hardware access is strictly mediated
- Only validated and filtered data is exposed (Unless an external driver is added by the suer)

---

## Design Philosophy

### 1. Safety First
- Deterministic behavior
- Defensive defaults
- No implicit side effects
- Clear failure boundaries

### 2. Minimal API Surface
ARK exposes only:
- Processed sensor outputs
- Flight state indicators
- Explicit lifecycle hooks
- Controlled configuration points

Everything else remains internal.

### 3. Strong Separation of Concerns
- **ARK Core** → flight-safe kernel logic  
- **Drivers** → hardware abstraction  
- **Flight OS** → scheduling and policy  
- **Payload** → non-critical computation  

---

## What ARK Provides

### Sensor Data Pipeline (For flight critical data)
- Raw sensor ingestion
- Filtering and validation
- Processed outputs (altitude, acceleration, etc.)
- Compile-time option for file-based data sources (simulation/testing)

### Flight State Support
- Centralized flight state interfaces
- Deterministic state evaluation inputs
- Designed to be driven by the Flight OS

### Execution Control
- Base `ARK` class initialization
- Driver registration
- Sensor pin / source configuration
- Mission parameter configuration (e.g. expected altitude)
- Payload attachment with strict execution constraints


## Architecture Overview

```
┌─────────────────────────────────────────────┐
│                Payload App                  │
│   (Non-critical logic, runs on free cycles) │
└─────────────────────▲───────────────────────┘
                      │
┌─────────────────────┴───────────────────────┐
│                 Flight OS                   │
│  • Registers drivers & subsystems           │
│  • Defines mission parameters               │
│  • Composes system configuration            │
└─────────────────────▲───────────────────────┘
                      │
┌─────────────────────┴───────────────────────┐
│                    ARK                      │
│  • Deterministic scheduling                 │
│  • Execution priority control               │
│  • Sensor filtering & validation            │
│  • Flight state interfaces                  │
│  • Safety gates & isolation                 │
└─────────────────────▲───────────────────────┘
                      │
┌─────────────────────┴───────────────────────┐
│             Hardware Drivers                │
│  • Sensors                                  │
│  • Actuators                                │
│  • Low-level peripherals                    │
└─────────────────────────────────────────────┘
```


---

## Project Structure

Current repository layout:
```
ARK/
├── core/ # Core ARK kernel (to be implemented)
├── docs/ # Architecture, specifications, design notes
├── examples/ # Example payloads and integrations
├── test/ # Unit and integration tests
├── tools/ # Build, simulation, and analysis tools
├── public/ # Logos and shared visual assets
├── README.md
```
---

## Usage Concept (High-Level)

```cpp
ARK ark;

/* Initialize kernel */
ark.init();

/* Register hardware drivers */
ark.addDriver(barometer);
ark.addDriver(accelerometer);

/* Configure sensor sources */
ark.configureSensors({
    .altitude = SENSOR_BARO,
    .accel    = SENSOR_IMU
});

/* Mission parameters */
ark.setExpectedAltitude(1200.0f);

/* Attach payload */
ark.attachPayload(payloadApp);

/* Hand control to Flight OS */
FlightOS::run(ark);
```
Payload execution never interferes with flight-critical execution.

---

## Intended Use Cases

- Rocket flight computers
- Experimental aerospace systems
- Safety-critical embedded research
- Flight simulation and validation
- Academic and R&D avionics platforms

---

## Contributing

ARK is developed as part of Team Ignition’s avionics stack.

Contributions are welcome in the form of:
- Design reviews
- Safety analysis
- Testing frameworks
- Simulation tooling
- Documentation improvements

Please open an issue or discussion before major changes.

---

## License

This project is licensed under the GNU General Public License v3
with additional restrictions.

Commercial use is prohibited without explicit written permission.

See the LICENSE file for details.

<div align="center"> <p><strong>ARK — build flight software that cannot fail silently.</strong></p> <img src="https://camo.githubusercontent.com/fbe262a7085aeff41e78915220b88792c6cd26c1964e2c4b91e312e948f2d7e8/68747470733a2f2f7465616d69676e6974696f6e2e73706163652f6c6f676f2f686561642d6c6f676f2e706e67" alt="Team Ignition" width="120"/> <p><sub>© Team Ignition, VIT Chennai</sub></p> </div>
