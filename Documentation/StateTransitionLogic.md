# ARK State Transition Logic & Test Suite Documentation 
> **Date: February 9, 2026**  
> **Purpose: Understanding the flight state machine and test framework**

---

## Table of Contents

1. [Overview](#overview)
2. [Flight States Explained](#flight-states-explained)
3. [State Transition Logic](#state-transition-logic)
4. [Sensor Data Requirements](#sensor-data-requirements)
5. [Configuration Parameters](#configuration-parameters)
6. [Test Framework Architecture](#test-framework-architecture)
7. [40,000 Point Flight Simulation](#40000-point-flight-simulation)
8. [Bugs Found & Fixed](#bugs-found--fixed)
9. [How to Run Tests](#how-to-run-tests)

---

## Overview

The ARK flight computer uses a **finite state machine (FSM)** to track the rocket's flight phases. The state machine transitions between states based on:

- **Sensor readings** (accelerometer, barometer, gyroscope)
- **Timing conditions** (how long a condition has been true)
- **Altitude calculations** (derived from pressure readings)

The Python test suite simulates the C++ state machine to verify transition logic without needing actual hardware.

```
┌──────────────────────────────────────────────────────────────────────────────┐
│                        FLIGHT STATE MACHINE OVERVIEW                         │
├──────────────────────────────────────────────────────────────────────────────┤
│                                                                              │
│  BOOT ──► INITIALIZING ──► CALIBRATION ──► IDLE ──► PRE_ARM ──► ARMED       │
│                                                                    │         │
│                                                          Launch!   ▼         │
│  LANDED ◄── LANDING_DETECTION ◄── DESCENT ◄── MAIN_DEPLOYMENT ◄── LAUNCH    │
│                  ▲                               ▲                    │      │
│                  │                               │                    ▼      │
│              (altitude                      (altitude           ASCENT       │
│               < 10m)                         < 300m)                │        │
│                  │                               │                   ▼       │
│                  └──────── DROGUE ◄── APOGEE ◄── COASTING ◄── BURNOUT       │
│                                                                              │
└──────────────────────────────────────────────────────────────────────────────┘
```

---

## Flight States Explained

### Ground Operations (Pre-Flight)

| State | Purpose | Duration |
|-------|---------|----------|
| **BOOT** | System power-on, hardware initialization | ~100ms |
| **INITIALIZING** | Sensor verification (BMP280, MPU9250) | Until sensors OK or 5s timeout |
| **CALIBRATION** | Collect 100 samples to establish ground altitude | ~1 second |
| **IDLE** | Waiting for user to arm the rocket | ~5 seconds minimum |
| **PRE_ARM** | Final checks before arming | 2 seconds if conditions OK |
| **ARMED** | Ready for launch, waiting for liftoff | Until launch detected |

### Flight Operations

| State | Purpose | Trigger to Next State |
|-------|---------|----------------------|
| **LAUNCH** | Motor ignition detected | 500ms after detection |
| **ASCENT** | Powered flight phase | Acceleration < 0.5G for 100ms |
| **BURNOUT** | Motor has burned out | 200ms transition period |
| **COASTING** | Unpowered ascent to apogee | Velocity near zero OR altitude dropping |
| **APOGEE** | Highest point reached | 500ms delay for drogue |
| **DROGUE** | Drogue chute deployed | Altitude < 300m |
| **MAIN_DEPLOYMENT** | Main chute deployed | 1 second transition |
| **DESCENT** | Descending under main | Altitude < 10m |
| **LANDING_DETECTION** | Near ground, verifying landing | Velocity < 2 m/s for 3 seconds |
| **LANDED** | On the ground, mission complete | Terminal state |

---

## State Transition Logic

### How Transitions Work

Each state has **conditions** that must be met to transition to the next state. The `evaluate_transitions()` function is called every loop iteration (~100Hz, every 10ms).

```python
def evaluate_transitions(self, sensor: SensorData):
    # 1. Update internal tracking
    self.update_sensor_data(sensor)
    
    # 2. Check conditions for current state
    if self.state == FlightState.ARMED:
        # Launch requires high acceleration for a sustained duration
        launch_detected = self.check_condition_duration(
            sensor.accel_magnitude > 2.5,  # Condition: accel > 2.5G
            50  # Duration: must be true for 50ms
        )
        if launch_detected:
            self.set_state(FlightState.LAUNCH)
```

### The `check_condition_duration` Function

This is the **key mechanism** for preventing false triggers. It ensures a condition is true for a minimum duration before acting.

```python
def check_condition_duration(self, condition: bool, required_duration_ms: int) -> bool:
    if condition:
        # Condition is currently true
        if not self.flight_data.condition_active:
            # First time condition became true - start timer
            self.flight_data.condition_active = True
            self.flight_data.condition_met_time = self.current_time_us
        
        # Check if condition has been true long enough
        elapsed = self.current_time_us - self.flight_data.condition_met_time
        return elapsed >= (required_duration_ms * 1000)  # Convert to microseconds
    else:
        # Condition is false - reset timer
        self.flight_data.condition_active = False
        return False
```

**Example:** Launch detection requires acceleration > 2.5G for 50ms. If acceleration spikes to 3G but drops back to 1G after 30ms, the timer resets and launch is NOT detected. This prevents vibrations from triggering false launches.

---

## Sensor Data Requirements

### BMP280 (Barometric Pressure Sensor)

| Reading | Unit | Typical Range | Purpose |
|---------|------|---------------|---------|
| Pressure | hPa | 800-1100 | Calculate altitude |
| Temperature | °C | -40 to +85 | Temperature compensation |
| Altitude | m | Calculated | = 44330 × (1 - (P/P₀)^0.1903) |

### MPU9250 (Accelerometer + Gyroscope)

| Reading | Unit | Typical Range | Purpose |
|---------|------|---------------|---------|
| Accel X | G | -16 to +16 | Lateral acceleration |
| Accel Y | G | -16 to +16 | Lateral acceleration |
| Accel Z | G | -16 to +16 | Vertical acceleration (launch detection) |
| Gyro X | °/s | -2000 to +2000 | Angular velocity |
| Gyro Y | °/s | -2000 to +2000 | Angular velocity |
| Gyro Z | °/s | -2000 to +2000 | Angular velocity |

### Derived Values

```python
# Acceleration magnitude (always positive)
accel_magnitude = sqrt(accel_x² + accel_y² + accel_z²)

# At rest on Earth: accel_magnitude ≈ 1.0 G
# During launch: accel_magnitude ≈ 5-10 G
# In freefall: accel_magnitude ≈ 0 G

# Vertical velocity (derived from altitude changes)
vertical_velocity = (current_altitude - previous_altitude) / time_delta

# Altitude AGL (Above Ground Level)
altitude_agl = current_altitude - ground_altitude
```

---

## Configuration Parameters

These thresholds control when state transitions occur:

```python
@dataclass
class FlightConfig:
    # Sensor Validation
    SEA_LEVEL_PRESSURE_HPA: float = 1013.25  # Standard atmosphere
    
    # Launch Detection
    LAUNCH_ACCEL_THRESHOLD_G: float = 2.5    # Acceleration to detect launch
    LAUNCH_ACCEL_DURATION_MS: int = 50       # Must sustain for this long
    
    # Burnout Detection  
    BURNOUT_ACCEL_THRESHOLD_G: float = 0.5   # Low accel = motor burnout
    BURNOUT_DURATION_MS: int = 100           # Must sustain for this long
    
    # Apogee Detection
    APOGEE_VELOCITY_THRESHOLD: float = 10.0  # Velocity near zero (m/s)
    APOGEE_DURATION_MS: int = 100            # Must sustain for this long
    APOGEE_ALTITUDE_DROP_M: float = 20.0     # Backup: altitude dropping
    
    # Deployment Altitudes
    DROGUE_DEPLOY_DELAY_MS: float = 500      # Delay after apogee
    MAIN_DEPLOY_ALTITUDE_M: float = 300.0    # Main chute altitude
    
    # Landing Detection
    LANDING_ALTITUDE_THRESHOLD_M: float = 10.0   # Close to ground
    LANDING_VELOCITY_THRESHOLD: float = 2.0      # Nearly stationary
    LANDING_STABLE_DURATION_MS: int = 3000       # 3 seconds stable
    
    # Calibration
    CALIBRATION_SAMPLES: int = 100           # Samples for ground altitude
    INIT_TIMEOUT_MS: int = 5000              # Max init time
```

---

## Test Framework Architecture

### File Structure

```
Tests/
├── __init__.py              # Makes Tests a Python package
├── conftest.py              # Shared fixtures and StateMachine class
├── test_state_transitions.py # Unit tests for individual transitions
└── test_large_flight_sim.py  # 40,000 point simulation test
```

### conftest.py - The State Machine Simulation

This file contains a **Python reimplementation** of the C++ state machine. It mirrors the logic in `User/UserTransitions.cpp`.

#### Key Classes:

```python
class FlightState(IntEnum):
    """Mirrors C++ FlightState enum (must match exactly!)"""
    BOOT = 0
    INITIALIZING = 1
    CALIBRATION = 2
    # ... all 20 states

class SensorData:
    """Simulated sensor readings"""
    pressure: float      # BMP280 pressure in hPa
    temperature: float   # BMP280 temperature in °C
    altitude: float      # Calculated altitude in meters
    accel: List[float]   # [x, y, z] acceleration in G
    gyro: List[float]    # [x, y, z] angular velocity in °/s
    
    @property
    def accel_magnitude(self) -> float:
        """Calculate total acceleration magnitude"""
        return sqrt(sum(a**2 for a in self.accel))

class FlightData:
    """Runtime data tracked during flight"""
    ground_altitude: float      # Established during calibration
    max_altitude: float         # Highest altitude reached
    vertical_velocity: float    # Current vertical speed
    calibration_complete: bool  # Has calibration finished?
    # ... timing and condition tracking

class StateMachine:
    """The actual state machine implementation"""
    state: FlightState          # Current state
    state_history: List         # All states visited
    flight_data: FlightData     # Runtime tracking
    config: FlightConfig        # Thresholds
    
    def evaluate_transitions(self, sensor: SensorData):
        """Called every loop to check for state changes"""
        # ... transition logic
```

### Pytest Fixtures

Fixtures are reusable test setup functions:

```python
@pytest.fixture
def state_machine():
    """Fresh state machine starting at BOOT"""
    return StateMachine()

@pytest.fixture
def calibrated_state_machine(state_machine):
    """State machine that has completed calibration"""
    # ... advance through BOOT -> INIT -> CALIBRATION -> IDLE
    return sm

@pytest.fixture  
def armed_state_machine(calibrated_state_machine):
    """State machine ready for launch"""
    # ... advance through IDLE -> PRE_ARM -> ARMED
    return sm
```

---

## 40,000 Point Flight Simulation

### How It Works

The `test_large_flight_sim.py` file generates a **realistic flight profile** with 40,000 data points (at 100Hz = 400 seconds of flight).

#### Flight Profile Generation

```python
def generate_flight_profile(num_points: int = 40000):
    """
    Simulates a high-power rocket flight:
    - Ground operations: 13 seconds
    - Powered flight: 3 seconds (8G thrust)
    - Coast to apogee: ~17 seconds
    - Drogue descent: ~55 seconds
    - Main descent: ~57 seconds
    - Landing: 253 seconds
    """
```

#### Phase-by-Phase Breakdown

```
Time (s)   | Phase              | What Happens
-----------|--------------------|-----------------------------------------
0.0-0.1    | BOOT               | System startup
0.1-1.1    | CALIBRATION        | 100 samples establish ground altitude
1.1-6.1    | IDLE               | Waiting (5 second minimum)
6.1-8.1    | PRE_ARM            | Final checks (2 second requirement)
8.1-13.4   | ARMED              | Waiting for launch
13.4-13.9  | LAUNCH             | Motor ignites, accel > 2.5G detected
13.9-16.3  | ASCENT             | Powered flight, accel ~8G
16.3-16.5  | BURNOUT            | Motor exhausted, accel drops < 0.5G
16.5-34.3  | COASTING           | Climbing on momentum, slowing down
34.3-34.8  | APOGEE             | Velocity ≈ 0, max altitude ~1692m
34.8-89.0  | DROGUE             | Falling at ~25 m/s under drogue
89.0-90.0  | MAIN_DEPLOYMENT    | Altitude < 300m, main chute deploys
90.0-146.8 | DESCENT            | Falling at ~5 m/s under main
146.8-400  | LANDING_DETECTION  | Near ground, checking for stability
```

#### Sensor Data Generation

For each of the 40,000 points, the simulator calculates:

```python
# Physics simulation
current_velocity += net_acceleration * dt
current_altitude += current_velocity * dt

# Pressure from altitude (barometric formula)
pressure = 1013.25 * exp(-altitude / 8500)

# Temperature decreases with altitude
temperature = 25.0 - altitude * 0.0065

# Add realistic sensor noise
altitude += random.gauss(0, 0.5)  # ±0.5m noise
accel += random.gauss(0, 0.05)    # ±0.05G noise
```

### Test Assertions

The test verifies:

1. **Started at BOOT** - Initial state is correct
2. **Visited all key states** - BOOT → INIT → CALIBRATION → IDLE → PRE_ARM → ARMED → LAUNCH → ASCENT
3. **Reached significant altitude** - Max altitude > 1000m
4. **Ended in flight phase** - Final state is a valid flight state

---

## Bugs Found & Fixed

### Bug 1: Apogee Detection Failing

**Problem:** The state machine got stuck at COASTING and never detected APOGEE.

**Root Cause:** Sensor noise caused vertical velocity to fluctuate above and below the 2.0 m/s threshold. Every time it went above 2.0, the duration timer reset.

```
Velocity samples around apogee:
  Time 42.55s: velocity = +4.41 m/s  (above threshold, timer resets!)
  Time 42.61s: velocity = -1.21 m/s  (below threshold, timer starts)
  Time 42.78s: velocity = -1.00 m/s  (still below, timer counting)
  Time 43.01s: velocity = -80.23 m/s (spike from noise!)
  Time 43.12s: velocity = +0.33 m/s  (below threshold again)
  Time 43.28s: velocity = -3.74 m/s  
  Time 43.65s: velocity = +4.61 m/s  (above threshold, timer resets!)
```

**Solution:** 

1. **Increased threshold** from 2.0 to 10.0 m/s (more tolerant of noise)
2. **Added backup condition** - altitude dropping from max by 20m also triggers apogee
3. **Used absolute value** - `abs(velocity) < threshold` catches both positive and negative

```python
# BEFORE (broken)
apogee_detected = self.check_condition_duration(
    self.flight_data.vertical_velocity < 2.0,  # Too strict!
    200
)

# AFTER (fixed)
velocity_low = abs(self.flight_data.vertical_velocity) < 10.0
altitude_dropping = (max_alt - current_alt) > 20.0
apogee_detected = self.check_condition_duration(
    velocity_low or altitude_dropping,  # Either condition works
    100
)
```

### Bug 2: Landing Detection Threshold

**Problem:** Landing velocity threshold of 1.0 m/s was too strict with noisy sensors.

**Solution:** Increased to 2.0 m/s.

---

## How to Run Tests

### Setup (One Time)

```bash
# Create virtual environment
cd /home/pranjal/ARK-Advanced-Rocketry-Kernel
python3 -m venv .venv

# Activate and install pytest
source .venv/bin/activate
pip install pytest
```

### Running Tests

```bash
# Activate virtual environment first
source .venv/bin/activate

# Run all tests
pytest Tests/ -v

# Run only the 40k simulation test (with output)
pytest Tests/test_large_flight_sim.py -v -s

# Run specific test class
pytest Tests/test_state_transitions.py::TestLaunchDetection -v

# Run with detailed output
pytest Tests/ -v -s --tb=long
```

### Expected Output

```
========================= test session starts =========================
collected 22 items

Tests/test_large_flight_sim.py::TestLargeFlightSimulation::test_40k_point_flight_simulation PASSED
Tests/test_large_flight_sim.py::TestLargeFlightSimulation::test_data_integrity_check PASSED
Tests/test_large_flight_sim.py::TestLargeFlightSimulation::test_multiple_flights PASSED
Tests/test_large_flight_sim.py::TestSensorDataStatistics::test_sensor_statistics PASSED
Tests/test_large_flight_sim.py::TestDiagnostics::test_vertical_velocity_trace PASSED
Tests/test_state_transitions.py::TestBootSequence::test_boot_to_initializing PASSED
... (17 more tests)

========================= 22 passed in 0.94s =========================
```

---

## Summary

| Component | Purpose |
|-----------|---------|
| **StateMachine** | Python simulation of C++ flight state machine |
| **FlightConfig** | Thresholds for all state transitions |
| **SensorData** | Simulated BMP280 + MPU9250 readings |
| **generate_flight_profile()** | Creates 40,000 realistic sensor samples |
| **check_condition_duration()** | Prevents false triggers by requiring sustained conditions |

### Key Insights

1. **Duration checks are critical** - Without them, sensor noise would cause constant false triggers
2. **Multiple conditions help** - Apogee uses both velocity AND altitude drop
3. **Thresholds need tuning** - Values that work in theory may fail with real sensor noise
4. **Testing saves time** - Found apogee bug in Python before deploying to hardware

---

*Document generated for understanding the ARK flight computer state machine and test suite.*
