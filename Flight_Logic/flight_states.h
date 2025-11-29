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

typedef enum
{
    FLIGHTSTATE_BOOT,
    FLIGHTSTATE_IDLE,
    FLIGHTSTATE_ARMED,
    FLIGHTSTATE_LAUNCH,
    FLIGHTSTATE_ASCENT,
    FLIGHTSTATE_CRUISING,
    FLIGHTSTATE_APOGEE,
    FLIGHTSTATE_DEPLOYMENT,
    FLIGHTSTATE_DESCENT,
    FLIGHTSTATE_LANDED,
    FLIGHTSTATE_FAILSAFE
} FlightState_t;

FlightState_t FlightState_HandleBoot();
FlightState_t FlightState_HandleIdle();
FlightState_t FlightState_HandleArmed();
FlightState_t FlightState_HandleLaunch();
FlightState_t FlightState_HandleAscent();
FlightState_t FlightState_HandleCruising();
FlightState_t FlightState_HandleApogee();
FlightState_t FlightState_HandleDeployment();
FlightState_t FlightState_HandleDescent();
FlightState_t FlightState_HandleLanded();
FlightState_t FlightState_HandleFailsafe();

#endif // FLIGHT_STATES_H
