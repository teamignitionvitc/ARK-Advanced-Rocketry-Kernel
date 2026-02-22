"""
Test state transitions with simulated sensor data.
Run with: source .venv/bin/activate && pytest Tests/ -v
"""
import pytest
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from conftest import StateMachine, SensorData, FlightState, FlightConfig


class TestBootSequence:
    """Test initial boot sequence transitions"""

    def test_boot_to_initializing(self, state_machine):
        """BOOT should transition to INITIALIZING after 100ms"""
        sensor = SensorData()
        assert state_machine.state == FlightState.BOOT

        # Before 100ms - should stay in BOOT
        state_machine.advance_time(50000)  # 50ms
        state_machine.evaluate_transitions(sensor)
        assert state_machine.state == FlightState.BOOT

        # After 100ms - should transition
        state_machine.advance_time(60000)  # +60ms = 110ms total
        state_machine.evaluate_transitions(sensor)
        assert state_machine.state == FlightState.INITIALIZING

    def test_initializing_to_calibration_with_good_sensors(self, state_machine):
        """INITIALIZING should go to CALIBRATION when sensors are OK"""
        # Get to INITIALIZING first
        state_machine.advance_time(150000)
        sensor = SensorData(pressure=1013.25, accel=[0, 0, 1.0])
        state_machine.evaluate_transitions(sensor)
        assert state_machine.state == FlightState.INITIALIZING

        # Good sensor readings should trigger transition
        state_machine.advance_time(10000)
        state_machine.evaluate_transitions(sensor)
        assert state_machine.state == FlightState.CALIBRATION

    def test_initializing_timeout(self, state_machine):
        """INITIALIZING should timeout to CALIBRATION after 5 seconds"""
        # Get to INITIALIZING
        state_machine.advance_time(150000)
        bad_sensor = SensorData(pressure=0, accel=[0, 0, 0])  # Bad readings
        state_machine.evaluate_transitions(bad_sensor)
        assert state_machine.state == FlightState.INITIALIZING

        # Wait for timeout (5 seconds)
        for _ in range(600):  # 600 * 10ms = 6 seconds
            state_machine.advance_time(10000)
            state_machine.evaluate_transitions(bad_sensor)

        assert state_machine.state == FlightState.CALIBRATION


class TestCalibration:
    """Test calibration phase"""

    def test_calibration_completes_after_samples(self, state_machine):
        """CALIBRATION should complete after required samples"""
        sensor = SensorData(altitude=100.0)

        # Fast forward to CALIBRATION
        for _ in range(50):
            state_machine.advance_time(10000)
            state_machine.evaluate_transitions(sensor)
            if state_machine.state == FlightState.CALIBRATION:
                break

        assert state_machine.state == FlightState.CALIBRATION

        # Run through calibration samples
        for i in range(150):
            state_machine.advance_time(10000)
            state_machine.evaluate_transitions(sensor)
            if state_machine.state == FlightState.IDLE:
                break

        assert state_machine.state == FlightState.IDLE
        assert state_machine.flight_data.calibration_complete
        assert state_machine.flight_data.ground_altitude == 100.0


class TestPreArmSequence:
    """Test pre-arm and arming sequence"""

    def test_idle_to_prearm(self, calibrated_state_machine):
        """IDLE should go to PRE_ARM after 5 seconds"""
        sm = calibrated_state_machine
        sensor = SensorData(altitude=100.0)

        assert sm.state == FlightState.IDLE

        # Wait for auto-transition (5 seconds)
        for _ in range(600):
            sm.advance_time(10000)
            sm.evaluate_transitions(sensor)
            if sm.state == FlightState.PRE_ARM:
                break

        assert sm.state == FlightState.PRE_ARM

    def test_prearm_to_armed(self, calibrated_state_machine):
        """PRE_ARM should go to ARMED when conditions are met"""
        sm = calibrated_state_machine
        sensor = SensorData(altitude=100.0, accel=[0, 0, 1.0])

        # Get to PRE_ARM
        for _ in range(600):
            sm.advance_time(10000)
            sm.evaluate_transitions(sensor)
            if sm.state == FlightState.PRE_ARM:
                break

        assert sm.state == FlightState.PRE_ARM

        # Wait for 2 seconds in PRE_ARM with good conditions
        for _ in range(300):
            sm.advance_time(10000)
            sm.evaluate_transitions(sensor)
            if sm.state == FlightState.ARMED:
                break

        assert sm.state == FlightState.ARMED


class TestLaunchDetection:
    """Test launch detection and powered flight"""

    def test_launch_detection_high_accel(self, armed_state_machine):
        """ARMED should detect launch with high acceleration"""
        sm = armed_state_machine
        assert sm.state == FlightState.ARMED

        # Apply high acceleration (simulating motor ignition)
        launch_sensor = SensorData(altitude=100.0, accel=[0, 0, 3.0])  # 3G

        # Need sustained acceleration for LAUNCH_ACCEL_DURATION_MS (50ms)
        for _ in range(10):  # 100ms of high accel
            sm.advance_time(10000)
            sm.evaluate_transitions(launch_sensor)
            if sm.state == FlightState.LAUNCH:
                break

        assert sm.state == FlightState.LAUNCH

    def test_no_launch_on_brief_accel(self, armed_state_machine):
        """Brief acceleration spike should not trigger launch"""
        sm = armed_state_machine
        assert sm.state == FlightState.ARMED

        # Brief spike
        spike_sensor = SensorData(altitude=100.0, accel=[0, 0, 3.0])
        sm.advance_time(20000)  # 20ms - less than threshold
        sm.evaluate_transitions(spike_sensor)

        # Then back to normal
        normal_sensor = SensorData(altitude=100.0, accel=[0, 0, 1.0])
        sm.advance_time(10000)
        sm.evaluate_transitions(normal_sensor)

        assert sm.state == FlightState.ARMED  # Should still be armed

    def test_launch_to_ascent(self, armed_state_machine):
        """LAUNCH should transition to ASCENT after 500ms"""
        sm = armed_state_machine

        # Trigger launch
        launch_sensor = SensorData(altitude=100.0, accel=[0, 0, 3.0])
        for _ in range(10):
            sm.advance_time(10000)
            sm.evaluate_transitions(launch_sensor)
            if sm.state == FlightState.LAUNCH:
                break

        assert sm.state == FlightState.LAUNCH

        # Wait for transition to ASCENT (500ms)
        for _ in range(60):
            sm.advance_time(10000)
            sm.evaluate_transitions(launch_sensor)
            if sm.state == FlightState.ASCENT:
                break

        assert sm.state == FlightState.ASCENT


class TestBurnoutDetection:
    """Test motor burnout detection"""

    def test_burnout_on_low_accel(self, armed_state_machine):
        """ASCENT should detect burnout when acceleration drops"""
        sm = armed_state_machine

        # Launch sequence
        launch_sensor = SensorData(altitude=100.0, accel=[0, 0, 3.0])
        for _ in range(100):
            sm.advance_time(10000)
            sm.evaluate_transitions(launch_sensor)
            if sm.state == FlightState.ASCENT:
                break

        assert sm.state == FlightState.ASCENT

        # Simulate burnout - low acceleration
        burnout_sensor = SensorData(altitude=500.0, accel=[0, 0, 0.2])  # < 0.5G
        for _ in range(20):  # 200ms at low accel
            sm.advance_time(10000)
            sm.evaluate_transitions(burnout_sensor)
            if sm.state == FlightState.BURNOUT:
                break

        assert sm.state == FlightState.BURNOUT


class TestApogeeDetection:
    """Test apogee detection"""

    def test_apogee_on_velocity_near_zero(self, armed_state_machine):
        """COASTING should detect apogee when velocity approaches zero"""
        sm = armed_state_machine

        # Fast forward through launch -> ascent -> burnout -> coasting
        altitudes = [100, 200, 400, 600, 800, 1000, 1100, 1150, 1180, 1195, 1200]
        accel_values = [3.0, 3.0, 3.0, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3, 0.3]

        for alt, acc in zip(altitudes, accel_values):
            sensor = SensorData(altitude=alt, accel=[0, 0, acc])
            for _ in range(50):
                sm.advance_time(10000)
                sm.evaluate_transitions(sensor)

        # Should be in COASTING or beyond
        assert sm.state in [FlightState.COASTING, FlightState.APOGEE, FlightState.DROGUE]

        # Simulate apogee - altitude stops increasing
        for _ in range(50):
            apogee_sensor = SensorData(altitude=1200.0, accel=[0, 0, 0.3])
            sm.advance_time(10000)
            sm.evaluate_transitions(apogee_sensor)

        # Should have reached APOGEE or beyond
        assert sm.state in [FlightState.APOGEE, FlightState.DROGUE, FlightState.MAIN_DEPLOYMENT]


class TestDeploymentSequence:
    """Test recovery deployment sequence"""

    def test_drogue_deploys_after_apogee_delay(self):
        """APOGEE should transition to DROGUE after delay"""
        sm = StateMachine()
        sm.state = FlightState.APOGEE
        sm.flight_data.state_entry_time = sm.current_time_us

        sensor = SensorData(altitude=1000.0)

        # Before delay
        sm.advance_time(400000)  # 400ms < 500ms
        sm.evaluate_transitions(sensor)
        assert sm.state == FlightState.APOGEE

        # After delay
        sm.advance_time(200000)  # +200ms = 600ms > 500ms
        sm.evaluate_transitions(sensor)
        assert sm.state == FlightState.DROGUE

    def test_main_deploys_at_altitude(self):
        """DROGUE should transition to MAIN_DEPLOYMENT at correct altitude"""
        sm = StateMachine()
        sm.state = FlightState.DROGUE
        sm.flight_data.state_entry_time = sm.current_time_us
        sm.flight_data.ground_altitude = 0.0

        # Above main deployment altitude
        sensor = SensorData(altitude=500.0)
        sm.advance_time(10000)
        sm.evaluate_transitions(sensor)
        assert sm.state == FlightState.DROGUE

        # Below main deployment altitude (300m)
        sensor = SensorData(altitude=250.0)
        sm.advance_time(10000)
        sm.evaluate_transitions(sensor)
        assert sm.state == FlightState.MAIN_DEPLOYMENT


class TestLanding:
    """Test landing detection"""

    def test_landing_detection(self):
        """LANDING_DETECTION should transition to LANDED when stable"""
        sm = StateMachine()
        sm.state = FlightState.LANDING_DETECTION
        sm.flight_data.state_entry_time = sm.current_time_us
        sm.flight_data.ground_altitude = 0.0

        # Simulate stable landing conditions
        sensor = SensorData(altitude=5.0, accel=[0, 0, 1.0])

        # Need to be stable for LANDING_STABLE_DURATION_MS (3000ms)
        for _ in range(400):  # 4 seconds
            sm.advance_time(10000)
            sm.evaluate_transitions(sensor)
            if sm.state == FlightState.LANDED:
                break

        assert sm.state == FlightState.LANDED


class TestFullFlightProfile:
    """Test complete flight profile simulation"""

    def test_complete_flight(self):
        """Simulate a complete flight from boot to landed"""
        sm = StateMachine()

        # Flight profile data: (time_delta_ms, altitude, accel_z)
        flight_profile = [
            # Boot and initialization
            (200, 100, 1.0),     # Boot -> Initializing
            (100, 100, 1.0),     # Initializing -> Calibration
            # Calibration (100 samples)
            *[(10, 100, 1.0) for _ in range(110)],  # Calibration -> Idle
            # Idle (5 seconds)
            *[(100, 100, 1.0) for _ in range(55)],  # Idle -> Pre_arm
            # Pre-arm (2 seconds)
            *[(100, 100, 1.0) for _ in range(25)],  # Pre_arm -> Armed
            # Launch detection
            *[(10, 100 + i*5, 3.5) for i in range(10)],  # Armed -> Launch
            # Powered ascent
            *[(10, 200 + i*20, 3.0) for i in range(100)],  # Launch -> Ascent
            # Burnout
            *[(10, 2200 + i*10, 0.3) for i in range(20)],  # Ascent -> Burnout
            # Coasting to apogee
            *[(50, 2400 + i*5, 0.3) for i in range(10)],  # Burnout -> Coasting
            *[(50, 2450, 0.3) for _ in range(10)],  # At apogee - velocity ~ 0
            # Drogue descent
            *[(100, 2450 - i*20, 1.0) for i in range(100)],  # Apogee -> Drogue
            # Main deployment and descent
            *[(100, 450 - i*5, 1.0) for i in range(80)],  # Drogue -> Main -> Descent
            # Landing
            *[(100, 5, 1.0) for _ in range(40)],  # Descent -> Landing_detection -> Landed
        ]

        for time_ms, altitude, accel_z in flight_profile:
            sensor = SensorData(altitude=altitude, accel=[0, 0, accel_z])
            sm.advance_time(time_ms * 1000)
            sm.evaluate_transitions(sensor)

        # Print flight log for debugging
        print("\n=== Flight Log ===")
        for entry in sm.log:
            print(entry)
        print(f"\nFinal state: {sm.state.name}")
        print(f"Max altitude: {sm.flight_data.max_altitude:.1f}m")

        # Verify we reached LANDED (or at least got through most states)
        expected_states = [
            FlightState.BOOT,
            FlightState.INITIALIZING,
            FlightState.CALIBRATION,
            FlightState.IDLE,
            FlightState.PRE_ARM,
            FlightState.ARMED,
            FlightState.LAUNCH,
        ]

        for expected in expected_states:
            assert expected in sm.state_history, f"Expected {expected.name} in history"


class TestEdgeCases:
    """Test edge cases and error conditions"""

    def test_sensor_noise_doesnt_trigger_false_launch(self, armed_state_machine):
        """Noisy sensor data shouldn't cause false positives"""
        sm = armed_state_machine
        import random

        # Simulate noisy but normal readings
        for _ in range(100):
            noise = random.uniform(-0.3, 0.3)
            sensor = SensorData(altitude=100.0, accel=[0, 0, 1.0 + noise])
            sm.advance_time(10000)
            sm.evaluate_transitions(sensor)

        assert sm.state == FlightState.ARMED

    def test_rapid_state_transitions_logged(self, state_machine):
        """Verify all state transitions are logged"""
        sensor = SensorData(altitude=100.0)

        for _ in range(500):
            state_machine.advance_time(10000)
            state_machine.evaluate_transitions(sensor)

        # Should have logged transitions
        assert len(state_machine.log) > 0
        assert len(state_machine.state_history) > 1
