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
* File: main.cpp
* Author: Kunsh Jain
* Created On: 30/11/25
*
* \brief Main entry point for A-R-K Flight Computer firmware.
* \details
*   - Initializes the FlightSystem (sensors + logger).
*   - Runs the main mission loop.
*   - Performs state machine dispatch via switch-case.
*   - Calls Harsh’s state handlers.
*   - Logs telemetry via DataLogger.
*
***************************************************************************************************
* HISTORY:
*
* +----- (NEW | MODify | ADD | DELete)
* |
* No#  | when      who        what
******+*********+***********+**********************************************************************
* 001  NEW       23/11/25   Anubhav          Initial creation
* 002  NEW       30/11/25   Kunsh Jain       Initial creation,Fixed Structure
* 003  MOD       30/11/25   MuhammadRamzy    Refactored to ARK Framework usage
**************************************************************************************************/

#include "../ARK/ARK.h"
#include <iostream>
#include <thread>
#include <chrono>

// Use namespaces for easier access
using namespace ARK::Core;
using namespace ARK::HAL;

// Global state for simulation
static FlightState_t gCurrentFlightState = FLIGHTSTATE_BOOT;

/**
 * @brief Helper to update mock sensors for simulation
 */
void UpdateSimulation(FlightSystem& system, int step)
{
    // Cast to mock sensors to set values
    auto* altimeter = static_cast<MockBarometer*>(system.GetAltimeter());
    auto* accelerometer = static_cast<MockAccelerometer*>(system.GetAccelerometer());

    if (!altimeter || !accelerometer) return;

    // Simulate flight profile based on step
    if (step < 10) {
        // Idle
        altimeter->SetAltitude(0.0f);
        accelerometer->SetAccelZ(9.8f);
    } else if (step < 20) {
        // Launch
        altimeter->SetAltitude((step - 10) * 2.0f);
        accelerometer->SetAccelZ(30.0f); // > 15.0 threshold
    } else if (step < 50) {
        // Ascent
        float t = step - 20;
        altimeter->SetAltitude(20.0f + t * 10.0f);
        accelerometer->SetAccelZ(20.0f);
    } else if (step < 70) {
        // Coasting / Burnout
        float t = step - 50;
        altimeter->SetAltitude(320.0f + t * 5.0f);
        accelerometer->SetAccelZ(-5.0f); // Decelerating
    } else if (step < 80) {
        // Apogee approach
        altimeter->SetAltitude(420.0f); // Peak
        accelerometer->SetAccelZ(-9.8f);
    } else if (step < 100) {
        // Descent
        float t = step - 80;
        altimeter->SetAltitude(420.0f - t * 20.0f);
        accelerometer->SetAccelZ(9.8f);
    } else {
        // Landed
        altimeter->SetAltitude(0.0f);
        accelerometer->SetAccelZ(9.8f);
    }
}

int main(void)
{
    // 1. Instantiate System (This sets the singleton instance)
    FlightSystem system;

    // 2. Register Hardware (Dependency Injection)
    // In a real user app, they would include their specific sensor drivers here
    // For this example, we use the Mock sensors provided by the framework
    system.RegisterAltimeter(new MockBarometer());
    system.RegisterAccelerometer(new MockAccelerometer());
    system.RegisterStorage(new MockStorage());

    // 3. Initialize System
    system.System_Init();

    std::cout << "[MAIN] Starting User App Simulation..." << std::endl;

    int step = 0;
    const int MAX_STEPS = 120;

    while (step < MAX_STEPS)
    {
        // Update simulation environment
        UpdateSimulation(system, step);

        /* Handle flight states */
        switch (gCurrentFlightState)
        {
            case FLIGHTSTATE_BOOT:
                gCurrentFlightState = FlightState_HandleBoot();
                break;

            case FLIGHTSTATE_IDLE:
                gCurrentFlightState = FlightState_HandleIdle();
                break;

            case FLIGHTSTATE_ARMED:
                gCurrentFlightState = FlightState_HandleArmed();
                break;

            case FLIGHTSTATE_LAUNCH:
                gCurrentFlightState = FlightState_HandleLaunch();
                break;

            case FLIGHTSTATE_ASCENT:
                gCurrentFlightState = FlightState_HandleAscent();
                break;

            case FLIGHTSTATE_CRUISING:
                gCurrentFlightState = FlightState_HandleCruising();
                break;

            case FLIGHTSTATE_APOGEE:
                gCurrentFlightState = FlightState_HandleApogee();
                break;

            case FLIGHTSTATE_DEPLOYMENT:
                gCurrentFlightState = FlightState_HandleDeployment();
                break;

            case FLIGHTSTATE_DESCENT:
                gCurrentFlightState = FlightState_HandleDescent();
                break;

            case FLIGHTSTATE_LANDED:
                gCurrentFlightState = FlightState_HandleLanded();
                break;

            case FLIGHTSTATE_FAILSAFE:
            default:
                gCurrentFlightState = FlightState_HandleFailsafe();
                break;
        }

        // Simulate time step
        std::this_thread::sleep_for(std::chrono::milliseconds(MAIN_LOOP_PERIOD_MS));
        step++;
    }
    
    std::cout << "[MAIN] Simulation Complete." << std::endl;
    return 0;
}
