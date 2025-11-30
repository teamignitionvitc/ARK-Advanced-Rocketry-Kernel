/*
                                        ::
                                        ::
                                        ::
                                        ::
                                        ::
    ..    ..........    :.      ::      ::     .........  ..    ..........    ...      .        
    ::    ::            : .:.   ::     .::.       ::      ::    ::       :    :: :.    :        
    ::    ::   ..:::    :   .:. ::    ::::::      ::      ::    ::       :    ::   ::  :        
    ::    ::......::    :      :::    ::::::      ::      ::    ::.......:    ::     :::        
                                      ::::::                                                    
                                      :.::.:                                                    
                         .::::          ::          ::::.                                      
                       .::::::::.       ::       .:::::::::                                    
                       ::::::::::::....::::.....:::::::::::                                    
                        .:::::::::::::::::::::::::::::::::.        

                    © Copyright of Ignition <Software Department>
                        All rights reserved
*/
/**************************************************************************************************
* File: IntegrationTests.cpp
* Author: MuhammadRamzy
* Created On: 30/11/25
*
* \brief Integration tests for ARK Flight System.
* \details
*   - Verifies state transitions under simulated conditions.
*   - Tests sensor integration and logic.
*
**************************************************************************************************/

#include "TestFramework.h"
#include "../ARK/ARK.h"
#include <memory>

using namespace ARK::Core;
using namespace ARK::HAL;

// Helper to reset singleton for tests (hacky but needed for singleton testing)
// In a real scenario, we might want dependency injection for the whole system or a Reset() method.
// For now, we just create a new one on stack which updates the static pointer, 
// but we need to be careful about destruction order.
// Actually, FlightSystem destructor clears the instance, so we are good if we scope it.

TEST_CASE(Test_NominalFlightProfile)
{
    FlightSystem system;
    
    // Register Mocks
    MockBarometer* altimeter = new MockBarometer();
    MockAccelerometer* accelerometer = new MockAccelerometer();
    system.RegisterAltimeter(altimeter);
    system.RegisterAccelerometer(accelerometer);
    system.RegisterStorage(new MockStorage());
    
    system.System_Init();
    
    // Initial State: BOOT -> IDLE
    FlightState_t state = FlightState_HandleBoot();
    ASSERT_EQ(FLIGHTSTATE_IDLE, state);
    
    // IDLE -> ARMED
    state = FlightState_HandleIdle();
    ASSERT_EQ(FLIGHTSTATE_ARMED, state);
    
    // ARMED -> LAUNCH
    // Simulate Launch Conditions
    accelerometer->SetAccelZ(30.0f); // > 15.0 threshold
    altimeter->SetAltitude(2.0f);    // > 1.0 threshold
    
    state = FlightState_HandleArmed();
    ASSERT_EQ(FLIGHTSTATE_LAUNCH, state);
    
    // LAUNCH -> ASCENT
    state = FlightState_HandleLaunch();
    ASSERT_EQ(FLIGHTSTATE_ASCENT, state);
    
    // ASCENT -> CRUISING (Burnout)
    accelerometer->SetAccelZ(-5.0f); // Deceleration
    state = FlightState_HandleAscent();
    ASSERT_EQ(FLIGHTSTATE_CRUISING, state);
    
    // CRUISING -> APOGEE
    // Simulate Apogee (negative velocity)
    // We need to simulate a few steps to establish velocity
    // FlightSystem calculates velocity based on diff from last read.
    // Let's manually tick it.
    
    // Step 1: Ascent
    altimeter->SetAltitude(100.0f);
    system.GetVelocityZ(); // Update internal state
    
    // Step 2: Peak
    altimeter->SetAltitude(105.0f);
    system.GetVelocityZ();
    
    // Step 3: Descent start (Apogee detected)
    altimeter->SetAltitude(104.0f); 
    // This should yield negative velocity
    
    state = FlightState_HandleCruising();
    ASSERT_EQ(FLIGHTSTATE_APOGEE, state);
    
    // APOGEE -> DEPLOYMENT
    state = FlightState_HandleApogee();
    ASSERT_EQ(FLIGHTSTATE_DEPLOYMENT, state);
    
    // DEPLOYMENT -> DESCENT
    state = FlightState_HandleDeployment();
    ASSERT_EQ(FLIGHTSTATE_DESCENT, state);
    
    // DESCENT -> LANDED
    // Simulate Landing (low altitude, low velocity)
    altimeter->SetAltitude(0.5f);
    // Need stable low velocity
    // Step 1
    altimeter->SetAltitude(0.5f);
    system.GetVelocityZ();
    // Step 2
    altimeter->SetAltitude(0.5f); 
    
    state = FlightState_HandleDescent();
    ASSERT_EQ(FLIGHTSTATE_LANDED, state);
}

TEST_CASE(Test_FullFlightSimulation)
{
    FlightSystem system;
    
    MockBarometer* altimeter = new MockBarometer();
    MockAccelerometer* accelerometer = new MockAccelerometer();
    system.RegisterAltimeter(altimeter);
    system.RegisterAccelerometer(accelerometer);
    system.RegisterStorage(new MockStorage());
    
    system.System_Init();
    
    FlightState_t currentState = FLIGHTSTATE_BOOT;
    
    // Simulation Parameters
    const int TOTAL_STEPS = 150;
    
    std::cout << "\n[SIL] Starting Software-in-Loop Simulation..." << std::endl;
    
    for (int step = 0; step < TOTAL_STEPS; ++step) {
        // --- Physics Model ---
        if (step < 10) {
            // Pre-launch: Stationary
            altimeter->SetAltitude(0.0f);
            accelerometer->SetAccelZ(9.8f);
        } else if (step < 30) {
            // Boost Phase: High Accel, Increasing Alt
            float t = step - 10;
            accelerometer->SetAccelZ(40.0f); 
            altimeter->SetAltitude(0.0f + 0.5f * 30.0f * t * t * 0.01f); // s = ut + 0.5at^2 (approx)
        } else if (step < 60) {
            // Coast Phase: Negative Accel (Drag/Gravity), Alt still increasing but slower
            float t = step - 30;
            accelerometer->SetAccelZ(-5.0f);
            altimeter->SetAltitude(100.0f + 20.0f * t * 0.1f); 
        } else if (step < 70) {
            // Apogee: Velocity -> 0
            accelerometer->SetAccelZ(-9.8f);
            altimeter->SetAltitude(150.0f); // Peak
        } else if (step < 100) {
            // Descent: Alt decreasing
            float t = step - 70;
            accelerometer->SetAccelZ(9.8f); // Under chute (approx equilibrium)
            altimeter->SetAltitude(150.0f - 5.0f * t * 0.1f);
        } else {
            // Landed
            altimeter->SetAltitude(0.0f);
            accelerometer->SetAccelZ(9.8f);
        }
        
        // --- State Machine Execution ---
        switch (currentState) {
            case FLIGHTSTATE_BOOT:
                currentState = FlightState_HandleBoot();
                break;
            case FLIGHTSTATE_IDLE:
                currentState = FlightState_HandleIdle();
                break;
            case FLIGHTSTATE_ARMED:
                currentState = FlightState_HandleArmed();
                break;
            case FLIGHTSTATE_LAUNCH:
                currentState = FlightState_HandleLaunch();
                break;
            case FLIGHTSTATE_ASCENT:
                currentState = FlightState_HandleAscent();
                break;
            case FLIGHTSTATE_CRUISING:
                currentState = FlightState_HandleCruising();
                break;
            case FLIGHTSTATE_APOGEE:
                currentState = FlightState_HandleApogee();
                break;
            case FLIGHTSTATE_DEPLOYMENT:
                currentState = FlightState_HandleDeployment();
                break;
            case FLIGHTSTATE_DESCENT:
                currentState = FlightState_HandleDescent();
                break;
            case FLIGHTSTATE_LANDED:
                currentState = FlightState_HandleLanded();
                break;
            default:
                currentState = FlightState_HandleFailsafe();
                break;
        }
        
        // --- Assertions at Key Points ---
        if (step == 5) ASSERT_EQ(FLIGHTSTATE_ARMED, currentState); // IDLE -> ARMED happens immediately
        if (step == 20) ASSERT_TRUE(currentState == FLIGHTSTATE_ASCENT || currentState == FLIGHTSTATE_LAUNCH);
        if (step == 50) ASSERT_EQ(FLIGHTSTATE_CRUISING, currentState);
        if (step == 80) ASSERT_TRUE(currentState == FLIGHTSTATE_DESCENT || currentState == FLIGHTSTATE_DEPLOYMENT);
        if (step == 140) ASSERT_EQ(FLIGHTSTATE_LANDED, currentState);
    }
    
    std::cout << "[SIL] Simulation Complete. Final State: " << currentState << std::endl;
    ASSERT_EQ(FLIGHTSTATE_LANDED, currentState);
}

TEST_CASE(Test_LaunchAbort)
{
    FlightSystem system;
    
    MockBarometer* altimeter = new MockBarometer();
    MockAccelerometer* accelerometer = new MockAccelerometer();
    system.RegisterAltimeter(altimeter);
    system.RegisterAccelerometer(accelerometer);
    system.RegisterStorage(new MockStorage());
    
    system.System_Init();
    
    // Get to ARMED
    FlightState_HandleBoot();
    FlightState_HandleIdle();
    
    // Simulate High Accel but NO Altitude change (e.g. dropped on pad)
    accelerometer->SetAccelZ(50.0f);
    altimeter->SetAltitude(0.0f);
    
    FlightState_t state = FlightState_HandleArmed();
    
    // Should NOT launch
    ASSERT_EQ(FLIGHTSTATE_ARMED, state);
}

int main()
{
    return ARK::Test::TestRegistry::GetInstance().RunAllTests();
}
