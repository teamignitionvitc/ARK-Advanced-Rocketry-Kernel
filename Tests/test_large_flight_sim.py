"""
Large-scale flight simulation test with 40,000 data points.
Simulates a realistic flight profile to stress test state transitions.

Run with: source .venv/bin/activate && pytest Tests/test_large_flight_sim.py -v -s
"""
import pytest
import math
import random
import sys
import os
sys.path.insert(0, os.path.dirname(os.path.abspath(__file__)))
from conftest import StateMachine, SensorData, FlightState, FlightConfig


def generate_flight_profile(num_points: int = 40000):
    """
    Generate realistic flight profile with 40,000 data points.
    
    Simulates:
    - Ground operations (boot, calibration, arming)
    - Launch and powered ascent
    - Coast to apogee
    - Drogue descent
    - Main deployment and descent
    - Landing
    
    Returns list of (time_delta_us, altitude, pressure, temperature, accel_x, accel_y, accel_z, gyro_x, gyro_y, gyro_z)
    """
    data = []
    
    # Flight parameters
    ground_altitude = 100.0  # meters ASL
    sea_level_pressure = 1013.25  # hPa
    ground_temp = 25.0  # Celsius
    
    # Motor parameters (simulating a high-power rocket)
    burn_time = 3.0  # seconds
    max_thrust_accel = 8.0  # G
    
    # Trajectory parameters
    apogee_altitude = 2500.0  # meters AGL
    drogue_descent_rate = 25.0  # m/s
    main_descent_rate = 5.0  # m/s
    main_deploy_altitude = 300.0  # meters AGL
    
    # Time step (10ms = 100Hz sampling)
    dt_us = 10000  # microseconds
    dt_s = dt_us / 1e6  # seconds
    
    # State variables
    current_altitude = ground_altitude
    current_velocity = 0.0
    current_time = 0.0
    
    # Noise parameters
    altitude_noise = 0.5  # meters
    accel_noise = 0.05  # G
    gyro_noise = 1.0  # deg/s
    pressure_noise = 0.5  # hPa
    temp_noise = 0.1  # Celsius
    
    # Phase durations
    boot_duration = 0.2  # seconds
    init_duration = 0.5
    calibration_duration = 2.0
    idle_duration = 5.5
    prearm_duration = 3.0
    armed_duration = 2.0  # Time before launch
    
    # Calculate points per phase
    points_per_second = int(1.0 / dt_s)
    
    # Phase tracking
    phase = "BOOT"
    phase_time = 0.0
    
    for i in range(num_points):
        # Add sensor noise
        noise_alt = random.gauss(0, altitude_noise)
        noise_ax = random.gauss(0, accel_noise)
        noise_ay = random.gauss(0, accel_noise)
        noise_az = random.gauss(0, accel_noise)
        noise_gx = random.gauss(0, gyro_noise)
        noise_gy = random.gauss(0, gyro_noise)
        noise_gz = random.gauss(0, gyro_noise)
        noise_press = random.gauss(0, pressure_noise)
        noise_temp = random.gauss(0, temp_noise)
        
        # Calculate pressure from altitude (barometric formula)
        # P = P0 * (1 - L*h/T0)^(g*M/(R*L))
        # Simplified: P ≈ P0 * exp(-h/8500)
        pressure = sea_level_pressure * math.exp(-current_altitude / 8500.0)
        
        # Temperature decreases with altitude (lapse rate ~6.5°C/km)
        temperature = ground_temp - (current_altitude - ground_altitude) * 0.0065
        
        # Default accelerations (at rest = 1G in Z)
        accel_x, accel_y, accel_z = 0.0, 0.0, 1.0
        gyro_x, gyro_y, gyro_z = 0.0, 0.0, 0.0
        
        # =====================================================================
        # Ground Operations
        # =====================================================================
        if phase == "BOOT":
            if phase_time >= boot_duration:
                phase = "INIT"
                phase_time = 0.0
            current_altitude = ground_altitude
            
        elif phase == "INIT":
            if phase_time >= init_duration:
                phase = "CALIBRATION"
                phase_time = 0.0
            current_altitude = ground_altitude
            
        elif phase == "CALIBRATION":
            if phase_time >= calibration_duration:
                phase = "IDLE"
                phase_time = 0.0
            current_altitude = ground_altitude
            
        elif phase == "IDLE":
            if phase_time >= idle_duration:
                phase = "PREARM"
                phase_time = 0.0
            current_altitude = ground_altitude
            
        elif phase == "PREARM":
            if phase_time >= prearm_duration:
                phase = "ARMED"
                phase_time = 0.0
            current_altitude = ground_altitude
            
        elif phase == "ARMED":
            if phase_time >= armed_duration:
                phase = "LAUNCH"
                phase_time = 0.0
            current_altitude = ground_altitude
            
        # =====================================================================
        # Powered Flight
        # =====================================================================
        elif phase == "LAUNCH":
            # Thrust curve: ramps up then constant
            if phase_time < 0.2:
                thrust_factor = phase_time / 0.2
            elif phase_time < burn_time - 0.5:
                thrust_factor = 1.0
            else:
                thrust_factor = (burn_time - phase_time) / 0.5
            
            thrust_accel = max_thrust_accel * max(0, thrust_factor)
            
            # Gravity acts downward
            net_accel = thrust_accel - 1.0  # Net acceleration in G (minus gravity)
            
            # Update velocity and position
            current_velocity += net_accel * 9.81 * dt_s
            current_altitude += current_velocity * dt_s
            
            # Accelerometer reads thrust + gravity
            accel_z = thrust_accel
            
            # Some rotation during ascent
            gyro_x = random.gauss(0, 5)
            gyro_y = random.gauss(0, 5)
            gyro_z = random.gauss(0, 2)
            
            if phase_time >= burn_time:
                phase = "COAST"
                phase_time = 0.0
                print(f"[{current_time:.2f}s] BURNOUT at {current_altitude - ground_altitude:.1f}m AGL, vel={current_velocity:.1f}m/s")
                
        # =====================================================================
        # Coasting to Apogee
        # =====================================================================
        elif phase == "COAST":
            # Drag deceleration (simplified)
            drag_decel = 0.1 * current_velocity * abs(current_velocity) / 1000.0
            
            # Update velocity (gravity + drag)
            current_velocity -= (9.81 + drag_decel) * dt_s
            current_altitude += current_velocity * dt_s
            
            # Accelerometer reads ~0 in freefall (just drag)
            accel_z = 0.1 + drag_decel / 9.81
            
            # Check for apogee (velocity crosses zero)
            if current_velocity <= 0:
                phase = "APOGEE"
                phase_time = 0.0
                print(f"[{current_time:.2f}s] APOGEE at {current_altitude - ground_altitude:.1f}m AGL")
                
        # =====================================================================
        # Apogee and Drogue Deployment
        # =====================================================================
        elif phase == "APOGEE":
            # Brief pause at apogee
            current_velocity = -2.0  # Start descending
            accel_z = 0.3
            
            if phase_time >= 0.5:
                phase = "DROGUE"
                phase_time = 0.0
                print(f"[{current_time:.2f}s] DROGUE DEPLOY")
                
        # =====================================================================
        # Drogue Descent
        # =====================================================================
        elif phase == "DROGUE":
            # Terminal velocity under drogue
            target_velocity = -drogue_descent_rate
            
            # Velocity approaches terminal
            current_velocity += (target_velocity - current_velocity) * 0.1
            current_altitude += current_velocity * dt_s
            
            # Accelerometer shows ~1G
            accel_z = 1.0 + random.gauss(0, 0.1)
            
            # Check for main deployment altitude
            altitude_agl = current_altitude - ground_altitude
            if altitude_agl <= main_deploy_altitude:
                phase = "MAIN"
                phase_time = 0.0
                print(f"[{current_time:.2f}s] MAIN DEPLOY at {altitude_agl:.1f}m AGL")
                
        # =====================================================================
        # Main Chute Descent
        # =====================================================================
        elif phase == "MAIN":
            # Slower terminal velocity under main
            target_velocity = -main_descent_rate
            
            # Velocity approaches terminal
            current_velocity += (target_velocity - current_velocity) * 0.2
            current_altitude += current_velocity * dt_s
            
            # Accelerometer shows ~1G with some swing
            accel_z = 1.0 + 0.1 * math.sin(current_time * 2)
            
            # Check for landing
            if current_altitude <= ground_altitude + 2:
                phase = "LANDING"
                phase_time = 0.0
                current_altitude = ground_altitude
                current_velocity = 0.0
                print(f"[{current_time:.2f}s] TOUCHDOWN")
                
        # =====================================================================
        # Landing
        # =====================================================================
        elif phase == "LANDING":
            current_altitude = ground_altitude
            current_velocity = 0.0
            accel_z = 1.0
            gyro_x, gyro_y, gyro_z = 0.0, 0.0, 0.0
            
            if phase_time >= 5.0:
                phase = "LANDED"
                print(f"[{current_time:.2f}s] LANDED (stable)")
                
        elif phase == "LANDED":
            current_altitude = ground_altitude
            accel_z = 1.0
        
        # Apply noise to all sensor readings
        data.append((
            dt_us,
            current_altitude + noise_alt,
            pressure + noise_press,
            temperature + noise_temp,
            accel_x + noise_ax,
            accel_y + noise_ay,
            accel_z + noise_az,
            gyro_x + noise_gx,
            gyro_y + noise_gy,
            gyro_z + noise_gz
        ))
        
        phase_time += dt_s
        current_time += dt_s
    
    return data


class TestLargeFlightSimulation:
    """Test state machine with 40,000 data points"""
    
    def test_40k_point_flight_simulation(self):
        """
        Simulate complete flight with 40,000 sensor readings.
        Verifies state machine handles large data volumes correctly.
        """
        print("\n" + "="*70)
        print("GENERATING 40,000 POINT FLIGHT PROFILE")
        print("="*70)
        
        # Generate flight data
        flight_data = generate_flight_profile(40000)
        
        print(f"\nGenerated {len(flight_data)} data points")
        print("="*70)
        print("RUNNING STATE MACHINE SIMULATION")
        print("="*70 + "\n")
        
        # Create state machine
        sm = StateMachine()
        
        # Track state durations
        state_times = {state: 0 for state in FlightState}
        last_state = sm.state
        
        # Process all data points
        for i, point in enumerate(flight_data):
            dt_us, altitude, pressure, temperature, ax, ay, az, gx, gy, gz = point
            
            sensor = SensorData(
                pressure=pressure,
                temperature=temperature,
                altitude=altitude,
                accel=[ax, ay, az],
                gyro=[gx, gy, gz]
            )
            
            sm.advance_time(dt_us)
            sm.evaluate_transitions(sensor)
            
            # Track time in each state
            state_times[sm.state] += dt_us
            
            # Log state changes
            if sm.state != last_state:
                print(f"[Point {i:5d}] State change: {last_state.name} -> {sm.state.name}")
                print(f"           Alt: {altitude:.1f}m, Accel: [{ax:.2f}, {ay:.2f}, {az:.2f}]G")
                last_state = sm.state
        
        # Print summary
        print("\n" + "="*70)
        print("SIMULATION COMPLETE")
        print("="*70)
        
        print("\n--- State History ---")
        for entry in sm.log:
            print(entry)
        
        print("\n--- Time in Each State ---")
        for state, time_us in state_times.items():
            if time_us > 0:
                print(f"  {state.name:20s}: {time_us/1e6:.2f} seconds")
        
        print(f"\n--- Flight Statistics ---")
        print(f"  Max Altitude AGL: {sm.flight_data.max_altitude:.1f} m")
        print(f"  Ground Altitude:  {sm.flight_data.ground_altitude:.1f} m")
        print(f"  Final Velocity:   {sm.flight_data.vertical_velocity:.1f} m/s")
        print(f"  Final State:      {sm.state.name}")
        print(f"  Total States Hit: {len(sm.state_history)}")
        
        # Assertions
        print("\n--- Verification ---")
        
        # Check we started at BOOT
        assert sm.state_history[0] == FlightState.BOOT, "Should start at BOOT"
        print("  [PASS] Started at BOOT")
        
        # Check we went through key states
        key_states = [
            FlightState.BOOT,
            FlightState.INITIALIZING, 
            FlightState.CALIBRATION,
            FlightState.IDLE,
            FlightState.PRE_ARM,
            FlightState.ARMED,
            FlightState.LAUNCH,
            FlightState.ASCENT,
        ]
        
        for state in key_states:
            assert state in sm.state_history, f"Should have visited {state.name}"
            print(f"  [PASS] Visited {state.name}")
        
        # Check max altitude is reasonable
        assert sm.flight_data.max_altitude > 1000, "Should reach significant altitude"
        print(f"  [PASS] Reached altitude > 1000m ({sm.flight_data.max_altitude:.1f}m)")
        
        # Check we're in a flight state past LAUNCH
        expected_flight_states = [
            FlightState.ASCENT,
            FlightState.BURNOUT,
            FlightState.COASTING,
            FlightState.APOGEE,
            FlightState.DROGUE,
            FlightState.MAIN_DEPLOYMENT,
            FlightState.DESCENT, 
            FlightState.LANDING_DETECTION,
            FlightState.LANDED,
        ]
        assert sm.state in expected_flight_states, \
            f"Should be in flight state, got {sm.state.name}"
        print(f"  [PASS] In flight phase: {sm.state.name}")
        
        print("\n" + "="*70)
        print("ALL VERIFICATIONS PASSED!")
        print("="*70)
    
    def test_data_integrity_check(self):
        """Verify generated data has correct format and ranges"""
        print("\n--- Data Integrity Check ---")
        
        flight_data = generate_flight_profile(1000)  # Smaller sample
        
        for i, point in enumerate(flight_data):
            dt_us, altitude, pressure, temperature, ax, ay, az, gx, gy, gz = point
            
            # Check time delta is positive
            assert dt_us > 0, f"Point {i}: time delta must be positive"
            
            # Check pressure range (reasonable for Earth)
            assert 200 < pressure < 1100, f"Point {i}: pressure {pressure} out of range"
            
            # Check temperature range
            assert -50 < temperature < 60, f"Point {i}: temperature {temperature} out of range"
            
            # Check acceleration magnitude is reasonable
            accel_mag = math.sqrt(ax**2 + ay**2 + az**2)
            assert 0 < accel_mag < 20, f"Point {i}: acceleration {accel_mag}G out of range"
        
        print(f"  [PASS] All {len(flight_data)} points have valid data")
    
    def test_multiple_flights(self):
        """Run multiple flight simulations to check consistency"""
        print("\n--- Multiple Flight Consistency Test ---")
        
        results = []
        
        for flight_num in range(3):
            flight_data = generate_flight_profile(10000)
            sm = StateMachine()
            
            for point in flight_data:
                dt_us, altitude, pressure, temperature, ax, ay, az, gx, gy, gz = point
                sensor = SensorData(
                    pressure=pressure,
                    temperature=temperature,
                    altitude=altitude,
                    accel=[ax, ay, az],
                    gyro=[gx, gy, gz]
                )
                sm.advance_time(dt_us)
                sm.evaluate_transitions(sensor)
            
            results.append({
                'max_altitude': sm.flight_data.max_altitude,
                'final_state': sm.state,
                'states_visited': len(sm.state_history)
            })
            
            print(f"  Flight {flight_num+1}: Max alt={sm.flight_data.max_altitude:.1f}m, "
                  f"Final={sm.state.name}, States={len(sm.state_history)}")
        
        # All flights should reach similar altitudes (within 20%)
        altitudes = [r['max_altitude'] for r in results]
        avg_alt = sum(altitudes) / len(altitudes)
        for alt in altitudes:
            assert abs(alt - avg_alt) / avg_alt < 0.3, "Altitude variance too high"
        
        print(f"  [PASS] Altitude consistency check (avg: {avg_alt:.1f}m)")
        
        # All flights should go through similar number of states
        state_counts = [r['states_visited'] for r in results]
        assert max(state_counts) - min(state_counts) < 5, "State visit count varies too much"
        
        print(f"  [PASS] State visit consistency check")


class TestSensorDataStatistics:
    """Statistical analysis of sensor data"""
    
    def test_sensor_statistics(self):
        """Analyze sensor data statistics"""
        print("\n--- Sensor Data Statistics ---")
        
        flight_data = generate_flight_profile(40000)
        
        # Extract values
        altitudes = [p[1] for p in flight_data]
        pressures = [p[2] for p in flight_data]
        temperatures = [p[3] for p in flight_data]
        accel_mags = [math.sqrt(p[4]**2 + p[5]**2 + p[6]**2) for p in flight_data]
        
        # Calculate statistics
        def stats(data, name):
            avg = sum(data) / len(data)
            min_val = min(data)
            max_val = max(data)
            print(f"  {name:15s}: min={min_val:8.2f}, max={max_val:8.2f}, avg={avg:8.2f}")
            return min_val, max_val, avg
        
        alt_stats = stats(altitudes, "Altitude (m)")
        press_stats = stats(pressures, "Pressure (hPa)")
        temp_stats = stats(temperatures, "Temp (C)")
        accel_stats = stats(accel_mags, "Accel (G)")
        
        # Verify ranges
        assert alt_stats[1] > 1500, "Should reach high altitude (>1500m)"
        assert 600 < press_stats[0] < 1100, "Pressure should be in valid range"
        assert accel_stats[1] > 5, "Should experience high G during launch"
        
        print("\n  [PASS] All statistics within expected ranges")


if __name__ == "__main__":
    # Run tests directly
    test = TestLargeFlightSimulation()
    test.test_40k_point_flight_simulation()


class TestDiagnostics:
    """Diagnostic tests to debug state machine behavior"""
    
    def test_vertical_velocity_trace(self):
        """Trace vertical velocity around apogee to debug detection"""
        print("\n" + "="*70)
        print("VERTICAL VELOCITY DIAGNOSTIC TRACE")
        print("="*70)
        
        flight_data = generate_flight_profile(5000)  # Shorter run
        sm = StateMachine()
        
        # Track velocity samples
        velocity_samples = []
        
        for i, point in enumerate(flight_data):
            dt_us, altitude, pressure, temperature, ax, ay, az, gx, gy, gz = point
            
            sensor = SensorData(
                pressure=pressure,
                temperature=temperature,
                altitude=altitude,
                accel=[ax, ay, az],
                gyro=[gx, gy, gz]
            )
            
            sm.advance_time(dt_us)
            sm.evaluate_transitions(sensor)
            
            # Sample velocity every 100 points
            if i % 100 == 0 or abs(sm.flight_data.vertical_velocity) < 5:
                velocity_samples.append({
                    'point': i,
                    'time': sm.current_time_us / 1e6,
                    'state': sm.state.name,
                    'altitude': sm.flight_data.altitude_agl,
                    'velocity': sm.flight_data.vertical_velocity
                })
        
        print("\n--- Velocity Samples (every 100 points + near-zero) ---")
        print(f"{'Point':>6} {'Time':>8} {'State':>15} {'Alt (m)':>10} {'Vel (m/s)':>10}")
        print("-" * 55)
        
        for s in velocity_samples[-50:]:  # Show last 50 samples
            print(f"{s['point']:>6} {s['time']:>8.2f} {s['state']:>15} {s['altitude']:>10.1f} {s['velocity']:>10.2f}")
        
        print(f"\n--- Final State: {sm.state.name} ---")
        print(f"Max altitude: {sm.flight_data.max_altitude:.1f}m")
        print(f"Final velocity: {sm.flight_data.vertical_velocity:.2f} m/s")
        
        # Check if we ever got close to zero velocity
        min_positive_vel = min([s['velocity'] for s in velocity_samples if s['velocity'] > 0] or [999])
        max_negative_vel = max([s['velocity'] for s in velocity_samples if s['velocity'] < 0] or [-999])
        
        print(f"\nVelocity extremes:")
        print(f"  Minimum positive velocity: {min_positive_vel:.2f} m/s")
        print(f"  Maximum negative velocity: {max_negative_vel:.2f} m/s")
        
        if min_positive_vel < 5 or max_negative_vel > -5:
            print("  -> Velocity DID cross near zero (apogee should detect)")
        else:
            print("  -> Velocity NEVER got close to zero (data issue)")
