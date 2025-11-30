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
* File: flight_states.cpp
* Author: Anubhav, Kunsh Jain
* Created On: 30/11/25
*
* \brief Flight state machine implementation for A-R-K Flight Computer.
* \details
*   - Implements the logic for transitioning between flight states.
*
***************************************************************************************************
* HISTORY:
*
* +----- (NEW | MODify | ADD | DELete)
* |
* No#  | when      who        what
******+*********+***********+**********************************************************************
* 001  NEW       30/11/25   Harsh   Initial creation
* 002  NEW       30/11/25   Kunsh Jain   Initial creation,Fixed Structure
**************************************************************************************************/

#include "flight_states.h"
#include "flight_system.h"

/**
 * @brief Implementation of BOOT state handler
 * @details Checks system health and transitions to IDLE when ready.
 *          This is the initial state on power-up.
 * @return FlightState_t IDLE if system healthy, BOOT otherwise
 */
FlightState_t FlightState_HandleBoot()
{
    if (FlightSystem::CheckHealth())
    {
        return FLIGHTSTATE_IDLE;
    }
    return FLIGHTSTATE_BOOT;
}

/**
 * @brief Implementation of IDLE state handler
 * @details Pre-launch standby. Waits for arming command or manual trigger.
 * @return FlightState_t ARMED (placeholder - implement arming logic)
 * @todo Implement actual arming logic (button press, command, etc.)
 */
FlightState_t FlightState_HandleIdle()
{
    return FLIGHTSTATE_ARMED;
}

/**
 * @brief Implementation of ARMED state handler
 * @details Monitors for launch detection based on acceleration and altitude.
 * @return FlightState_t LAUNCH when liftoff detected
 * @todo Implement actual launch detection using GetAccelZ() and GetAltitude()
 */
FlightState_t FlightState_HandleArmed()
{
    return FLIGHTSTATE_LAUNCH;
}

/**
 * @brief Implementation of LAUNCH state handler
 * @details Confirms liftoff and transitions to powered ascent.
 * @return FlightState_t ASCENT
 */
FlightState_t FlightState_HandleLaunch()
{
    return FLIGHTSTATE_ASCENT;
}

/**
 * @brief Implementation of ASCENT state handler
 * @details Monitors powered flight phase. Detects motor burnout.
 * @return FlightState_t CRUISING when motor burnout detected
 * @todo Implement motor burnout detection logic
 */
FlightState_t FlightState_HandleAscent()
{
    return FLIGHTSTATE_CRUISING;
}

/**
 * @brief Implementation of CRUISING state handler
 * @details Monitors unpowered ascent. Detects apogee approach.
 * @return FlightState_t APOGEE when peak altitude detected
 * @todo Implement apogee detection using velocity and altitude
 */
FlightState_t FlightState_HandleCruising()
{
    return FLIGHTSTATE_APOGEE;
}

/**
 * @brief Implementation of APOGEE state handler
 * @details Confirms peak altitude and initiates deployment sequence.
 * @return FlightState_t DEPLOYMENT
 */
FlightState_t FlightState_HandleApogee()
{
    return FLIGHTSTATE_DEPLOYMENT;
}

/**
 * @brief Implementation of DEPLOYMENT state handler
 * @details Manages recovery system deployment (parachute, etc.).
 * @return FlightState_t DESCENT when deployment confirmed
 * @todo Implement deployment confirmation logic
 */
FlightState_t FlightState_HandleDeployment()
{
    return FLIGHTSTATE_DESCENT;
}

/**
 * @brief Implementation of DESCENT state handler
 * @details Monitors controlled descent. Detects landing.
 * @return FlightState_t LANDED when touchdown detected
 * @todo Implement landing detection using altitude stability
 */
FlightState_t FlightState_HandleDescent()
{
    return FLIGHTSTATE_LANDED;
}

/**
 * @brief Implementation of LANDED state handler
 * @details Post-flight terminal state. Remains in LANDED.
 * @return FlightState_t Always LANDED
 */
FlightState_t FlightState_HandleLanded()
{
    return FLIGHTSTATE_LANDED;
}
/**
 * @brief Implementation of FAILSAFE state handler
 * @details Emergency terminal state for system failures.
 * @return FlightState_t Always FAILSAFE
 */
FlightState_t FlightState_HandleFailsafe()
{
    return FLIGHTSTATE_FAILSAFE;
}