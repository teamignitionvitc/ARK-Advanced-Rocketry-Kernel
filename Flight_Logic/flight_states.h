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
* File: flight_states.h
* Author: Anubhav, Kunsh Jain
* Created On: 30/11/25
*
* \brief Header for flight state machine logic.
* \details
*   - Contains declarations for flight state transition functions.
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

#ifndef FLIGHT_STATES_H
#define FLIGHT_STATES_H

#include "config.h"
#include "flight_system.h"

/**
 * @enum FlightState_t
 * @brief Enumeration of all possible flight states
 * @details Defines the complete set of states in the flight state machine.
 *          Each state represents a distinct phase of the rocket flight lifecycle.
 */
typedef enum
{
    FLIGHTSTATE_BOOT,        /**< System initialization and health checks */
    FLIGHTSTATE_IDLE,        /**< Pre-launch standby, awaiting arming */
    FLIGHTSTATE_ARMED,       /**< Ready for launch, monitoring for liftoff */
    FLIGHTSTATE_LAUNCH,      /**< Liftoff detected, initial ascent phase */
    FLIGHTSTATE_ASCENT,      /**< Powered flight, motor burning */
    FLIGHTSTATE_CRUISING,    /**< Coasting phase, unpowered ascent */
    FLIGHTSTATE_APOGEE,      /**< Peak altitude reached, preparing deployment */
    FLIGHTSTATE_DEPLOYMENT,  /**< Parachute/recovery system deployment */
    FLIGHTSTATE_DESCENT,     /**< Controlled descent under parachute */
    FLIGHTSTATE_LANDED,      /**< Touchdown confirmed, post-flight mode */
    FLIGHTSTATE_FAILSAFE     /**< Emergency mode for system errors */
} FlightState_t;

/**
 * @brief Handle BOOT state logic
 * @details Performs system initialization and health checks.
 *          Transitions to IDLE when all systems are healthy.
 * @return FlightState_t Next state (IDLE if healthy, BOOT if still initializing)
 * @see FlightSystem::CheckHealth()
 */
FlightState_t FlightState_HandleBoot();

/**
 * @brief Handle IDLE state logic
 * @details Pre-launch standby state. Waits for arming command.
 * @return FlightState_t Next state (ARMED when armed, IDLE otherwise)
 */
FlightState_t FlightState_HandleIdle();

/**
 * @brief Handle ARMED state logic
 * @details Ready for launch. Monitors sensors for liftoff detection.
 * @return FlightState_t Next state (LAUNCH when liftoff detected, ARMED otherwise)
 * @note Launch detection uses THRESHOLD_ACC and THRESHOLD_Altitude from config.h
 */
FlightState_t FlightState_HandleArmed();

/**
 * @brief Handle LAUNCH state logic
 * @details Initial liftoff phase. Confirms launch and transitions to ascent.
 * @return FlightState_t Next state (ASCENT when launch confirmed)
 */
FlightState_t FlightState_HandleLaunch();

/**
 * @brief Handle ASCENT state logic
 * @details Powered flight phase during motor burn.
 * @return FlightState_t Next state (CRUISING when motor burnout detected)
 */
FlightState_t FlightState_HandleAscent();

/**
 * @brief Handle CRUISING state logic
 * @details Unpowered ascent phase after motor burnout.
 * @return FlightState_t Next state (APOGEE when peak altitude detected)
 */
FlightState_t FlightState_HandleCruising();

/**
 * @brief Handle APOGEE state logic
 * @details Peak altitude reached. Prepares for recovery deployment.
 * @return FlightState_t Next state (DEPLOYMENT when apogee confirmed)
 * @note Uses THRESHOLD_APOGEE from config.h for confirmation
 */
FlightState_t FlightState_HandleApogee();

/**
 * @brief Handle DEPLOYMENT state logic
 * @details Manages parachute/recovery system deployment.
 * @return FlightState_t Next state (DESCENT when deployment confirmed)
 */
FlightState_t FlightState_HandleDeployment();

/**
 * @brief Handle DESCENT state logic
 * @details Controlled descent under parachute. Monitors for landing.
 * @return FlightState_t Next state (LANDED when touchdown detected)
 * @note Uses TIME_LANDING_STABLE_MS and THRESH_LANDING_ALT_NOISE from config.h
 */
FlightState_t FlightState_HandleDescent();

/**
 * @brief Handle LANDED state logic
 * @details Post-flight state after touchdown confirmation.
 * @return FlightState_t Always returns LANDED (terminal state)
 */
FlightState_t FlightState_HandleLanded();

/**
 * @brief Handle FAILSAFE state logic
 * @details Emergency state for system errors or anomalies.
 * @return FlightState_t Always returns FAILSAFE (terminal state)
 * @note This state can be entered from any other state on critical failure
 */
FlightState_t FlightState_HandleFailsafe();

#endif // FLIGHT_STATES_H
