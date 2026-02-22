"""
Pytest configuration and shared fixtures for ARK state transition tests.
"""
import pytest
from dataclasses import dataclass, field
from typing import List, Optional
from enum import IntEnum
import math


class FlightState(IntEnum):
    """Mirror of C++ FlightState enum"""
    BOOT = 0
    INITIALIZING = 1
    CALIBRATION = 2
    IDLE = 3
    PRE_ARM = 4
    ARMED = 5
    LAUNCH = 6
    ASCENT = 7
    BURNOUT = 8
    SEPARATION = 9
    CRUISING = 10
    COASTING = 11
    APOGEE = 12
    DROGUE = 13
    DOUBLE_DEPLOYMENT = 14
    MAIN_DEPLOYMENT = 15
    DESCENT = 16
    LANDING_DETECTION = 17
    LANDED = 18
    FAILSAFE = 19


@dataclass
class SensorData:
    """Simulated sensor readings"""
    pressure: float = 1013.25
    temperature: float = 25.0
    altitude: float = 0.0
    accel: List[float] = field(default_factory=lambda: [0.0, 0.0, 1.0])
    gyro: List[float] = field(default_factory=lambda: [0.0, 0.0, 0.0])

    @property
    def accel_magnitude(self) -> float:
        return math.sqrt(sum(a**2 for a in self.accel))


@dataclass
class FlightConfig:
    """Flight transition configuration"""
    SEA_LEVEL_PRESSURE_HPA: float = 1013.25
    LAUNCH_ACCEL_THRESHOLD_G: float = 2.5
    LAUNCH_ACCEL_DURATION_MS: int = 50
    BURNOUT_ACCEL_THRESHOLD_G: float = 0.5
    BURNOUT_DURATION_MS: int = 100
    APOGEE_VELOCITY_THRESHOLD: float = 10.0  # More tolerant for noisy sensors
    APOGEE_DURATION_MS: int = 100  # Shorter window
    APOGEE_ALTITUDE_DROP_M: float = 20.0  # Alt drop from max triggers apogee
    DROGUE_DEPLOY_DELAY_MS: float = 500
    MAIN_DEPLOY_ALTITUDE_M: float = 300.0
    LANDING_ALTITUDE_THRESHOLD_M: float = 10.0
    LANDING_VELOCITY_THRESHOLD: float = 2.0  # More tolerant
    LANDING_STABLE_DURATION_MS: int = 3000
    CALIBRATION_SAMPLES: int = 100
    INIT_TIMEOUT_MS: int = 5000


@dataclass
class FlightData:
    """Runtime flight data"""
    altitude: float = 0.0
    pressure: float = 0.0
    temperature: float = 0.0
    accel_magnitude: float = 1.0
    ground_altitude: float = 0.0
    altitude_agl: float = 0.0
    max_altitude: float = 0.0
    vertical_velocity: float = 0.0
    last_altitude: float = 0.0
    last_altitude_time: int = 0
    calibration_count: int = 0
    calibration_complete: bool = False
    sensors_ready: bool = False
    state_entry_time: int = 0
    condition_met_time: int = 0
    condition_active: bool = False


class StateMachine:
    """Python simulation of ARK state machine for testing."""

    def __init__(self, config: Optional[FlightConfig] = None):
        self.config = config or FlightConfig()
        self.state = FlightState.BOOT
        self.flight_data = FlightData()
        self.current_time_us = 0
        self.state_history: List[FlightState] = [FlightState.BOOT]
        self.log: List[str] = []

    def set_time(self, time_us: int):
        self.current_time_us = time_us

    def advance_time(self, delta_us: int):
        self.current_time_us += delta_us

    def time_in_state(self) -> int:
        return self.current_time_us - self.flight_data.state_entry_time

    def set_state(self, new_state: FlightState):
        old_state = self.state
        self.state = new_state
        self.flight_data.state_entry_time = self.current_time_us
        self.flight_data.condition_active = False
        self.state_history.append(new_state)
        self.log.append(f"[{self.current_time_us/1e6:.3f}s] {old_state.name} -> {new_state.name}")

    def check_condition_duration(self, condition: bool, required_duration_ms: int) -> bool:
        if condition:
            if not self.flight_data.condition_active:
                self.flight_data.condition_active = True
                self.flight_data.condition_met_time = self.current_time_us
            return (self.current_time_us - self.flight_data.condition_met_time) >= (required_duration_ms * 1000)
        else:
            self.flight_data.condition_active = False
            return False

    def update_sensor_data(self, sensor: SensorData):
        self.flight_data.pressure = sensor.pressure
        self.flight_data.temperature = sensor.temperature
        self.flight_data.altitude = sensor.altitude
        self.flight_data.accel_magnitude = sensor.accel_magnitude
        self.flight_data.altitude_agl = sensor.altitude - self.flight_data.ground_altitude

        if self.flight_data.altitude_agl > self.flight_data.max_altitude:
            self.flight_data.max_altitude = self.flight_data.altitude_agl

        if self.flight_data.last_altitude_time > 0:
            dt = (self.current_time_us - self.flight_data.last_altitude_time) / 1e6
            if dt > 0.001:
                self.flight_data.vertical_velocity = (
                    self.flight_data.altitude_agl - self.flight_data.last_altitude
                ) / dt
        self.flight_data.last_altitude = self.flight_data.altitude_agl
        self.flight_data.last_altitude_time = self.current_time_us

    def evaluate_transitions(self, sensor: SensorData):
        """Evaluate state transitions based on sensor data."""
        self.update_sensor_data(sensor)

        if self.state == FlightState.BOOT:
            if self.time_in_state() > 100000:
                self.set_state(FlightState.INITIALIZING)

        elif self.state == FlightState.INITIALIZING:
            bmp_ok = 800.0 < sensor.pressure < 1200.0
            mpu_ok = 0.5 < sensor.accel_magnitude < 2.0
            if bmp_ok and mpu_ok:
                self.flight_data.sensors_ready = True
                self.set_state(FlightState.CALIBRATION)
            elif self.time_in_state() > self.config.INIT_TIMEOUT_MS * 1000:
                self.set_state(FlightState.CALIBRATION)

        elif self.state == FlightState.CALIBRATION:
            self.flight_data.calibration_count += 1
            if self.flight_data.calibration_count >= self.config.CALIBRATION_SAMPLES:
                if not self.flight_data.calibration_complete:
                    self.flight_data.ground_altitude = sensor.altitude
                    self.flight_data.calibration_complete = True
                    self.set_state(FlightState.IDLE)

        elif self.state == FlightState.IDLE:
            if self.time_in_state() > 5000000:
                self.set_state(FlightState.PRE_ARM)

        elif self.state == FlightState.PRE_ARM:
            altitude_ok = abs(self.flight_data.altitude_agl) < 10.0
            accel_ok = abs(sensor.accel_magnitude - 1.0) < 0.2
            if altitude_ok and accel_ok and self.time_in_state() > 2000000:
                self.set_state(FlightState.ARMED)

        elif self.state == FlightState.ARMED:
            launch_detected = self.check_condition_duration(
                sensor.accel_magnitude > self.config.LAUNCH_ACCEL_THRESHOLD_G,
                self.config.LAUNCH_ACCEL_DURATION_MS
            )
            if launch_detected:
                self.set_state(FlightState.LAUNCH)

        elif self.state == FlightState.LAUNCH:
            if self.time_in_state() > 500000:
                self.set_state(FlightState.ASCENT)

        elif self.state == FlightState.ASCENT:
            burnout_detected = self.check_condition_duration(
                sensor.accel_magnitude < self.config.BURNOUT_ACCEL_THRESHOLD_G,
                self.config.BURNOUT_DURATION_MS
            )
            if burnout_detected:
                self.set_state(FlightState.BURNOUT)

        elif self.state == FlightState.BURNOUT:
            if self.time_in_state() > 200000:
                self.set_state(FlightState.COASTING)

        elif self.state == FlightState.COASTING:
            velocity_low = abs(self.flight_data.vertical_velocity) < self.config.APOGEE_VELOCITY_THRESHOLD
            altitude_dropping = (self.flight_data.max_altitude - self.flight_data.altitude_agl) > self.config.APOGEE_ALTITUDE_DROP_M
            
            apogee_detected = self.check_condition_duration(
                velocity_low or altitude_dropping,
                self.config.APOGEE_DURATION_MS
            )
            if apogee_detected:
                self.set_state(FlightState.APOGEE)

        elif self.state == FlightState.APOGEE:
            if self.time_in_state() > self.config.DROGUE_DEPLOY_DELAY_MS * 1000:
                self.set_state(FlightState.DROGUE)

        elif self.state == FlightState.DROGUE:
            if self.flight_data.altitude_agl < self.config.MAIN_DEPLOY_ALTITUDE_M:
                self.set_state(FlightState.MAIN_DEPLOYMENT)

        elif self.state == FlightState.MAIN_DEPLOYMENT:
            if self.time_in_state() > 1000000:
                self.set_state(FlightState.DESCENT)

        elif self.state == FlightState.DESCENT:
            if self.flight_data.altitude_agl < self.config.LANDING_ALTITUDE_THRESHOLD_M:
                self.set_state(FlightState.LANDING_DETECTION)

        elif self.state == FlightState.LANDING_DETECTION:
            velocity_low = abs(self.flight_data.vertical_velocity) < self.config.LANDING_VELOCITY_THRESHOLD
            accel_stable = abs(sensor.accel_magnitude - 1.0) < 0.3
            landed_detected = self.check_condition_duration(
                velocity_low and accel_stable,
                self.config.LANDING_STABLE_DURATION_MS
            )
            if landed_detected:
                self.set_state(FlightState.LANDED)


@pytest.fixture
def state_machine():
    """Create a fresh state machine for testing"""
    return StateMachine()


@pytest.fixture
def calibrated_state_machine():
    """Create a state machine that has completed calibration"""
    sm = StateMachine()
    sensor = SensorData(altitude=100.0)
    for _ in range(200):
        sm.advance_time(10000)
        sm.evaluate_transitions(sensor)
        if sm.state == FlightState.IDLE:
            break
    return sm


@pytest.fixture
def armed_state_machine():
    """Create a state machine in ARMED state"""
    sm = StateMachine()
    sensor = SensorData(altitude=100.0)
    for _ in range(1000):
        sm.advance_time(10000)
        sm.evaluate_transitions(sensor)
        if sm.state == FlightState.ARMED:
            break
    return sm
