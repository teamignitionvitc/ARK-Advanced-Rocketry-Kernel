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
* File: main.h
* Author: Anubhav, Kunsh Jain
* Created On: 30/11/25
*
* \brief Main entry point for A-R-K Flight Computer firmware.
* \details
*   - Initializes the FlightSystem (sensors + logger).
*   - Runs the main mission loop.
*   - Performs state machine dispatch via switch-case.
*
***************************************************************************************************
* HISTORY:
*
* +----- (NEW | MODify | ADD | DELete)
* |
* No#  | when      who        what
******+*********+***********+**********************************************************************
* 001  NEW       30/11/25   Kunsh Jain   Initial creation
**************************************************************************************************/

/**
 * @file main.h
 * @brief Main header file for ARK flight computer firmware
 * @details This header includes all necessary dependencies for the main flight control loop.
 *          It brings together the configuration, flight system, and state machine components.
 * 
 * @author Anubhav, Kunsh Jain
 * @date 30/11/25
 * @version 1.0.0
 */

#ifndef MAIN_H
#define MAIN_H

/**
 * @brief Global configuration parameters
 * @details Includes all threshold values, timing parameters, and system constants
 * @see config.h
 */
#include "config.h"

/**
 * @brief Flight system management class
 * @details Provides interface for sensor readings, actuator control, and data logging
 * @see FlightSystem
 */
#include "Flight_Logic/flight_system.h"

/**
 * @brief Flight state machine definitions
 * @details Defines all flight states and their handler functions
 * @see FlightState_t
 * @see FlightState_HandleBoot()
 */
#include "Flight_Logic/flight_states.h"

#endif // MAIN_H
