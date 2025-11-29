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

FlightState_t FlightState_HandleBoot()
{
    if (FlightSystem::CheckHealth())
    {
        return FLIGHTSTATE_IDLE;
    }
    return FLIGHTSTATE_BOOT;
}

FlightState_t FlightState_HandleIdle()
{
    return FLIGHTSTATE_ARMED;
}

FlightState_t FlightState_HandleArmed()
{
    return FLIGHTSTATE_LAUNCH;
}

FlightState_t FlightState_HandleLaunch()
{
    return FLIGHTSTATE_ASCENT;
}

FlightState_t FlightState_HandleAscent()
{
    return FLIGHTSTATE_CRUISING;
}

FlightState_t FlightState_HandleCruising()
{
    return FLIGHTSTATE_APOGEE;
}

FlightState_t FlightState_HandleApogee()
{
    return FLIGHTSTATE_DEPLOYMENT;
}

FlightState_t FlightState_HandleDeployment()
{
    return FLIGHTSTATE_DESCENT;
}

FlightState_t FlightState_HandleDescent()
{
    return FLIGHTSTATE_LANDED;
}

FlightState_t FlightState_HandleLanded()
{
    return FLIGHTSTATE_LANDED;
}
FlightState_t FlightState_HandleFailsafe()
{
    return FLIGHTSTATE_FAILSAFE;
}