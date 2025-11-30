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
* File: StateMachine.cpp
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
* No#  | when      who            what
* ******+*********+**************+*****************************************************************
* 001  NEW       30/11/25       Harsh            Initial creation
* 002  NEW       30/11/25       Kunsh Jain       Initial creation,Fixed Structure
* 003  MOD       30/11/25       MuhammadRamzy    Refactored to ARK Framework namespace
**************************************************************************************************/

#include "StateMachine.h"
#include "FlightSystem.h"
#include <iostream>
#include <cmath>

namespace ARK {
namespace Core {

// Helper to get system instance
static FlightSystem& System() {
    return FlightSystem::GetInstance();
}

FlightState_t FlightState_HandleBoot()
{
    System().LogData(FLIGHTSTATE_BOOT);
    
    if (FlightSystem::CheckHealth())
    {
        std::cout << "[STATE] BOOT -> IDLE" << std::endl;
        return FLIGHTSTATE_IDLE;
    }
    return FLIGHTSTATE_BOOT;
}

FlightState_t FlightState_HandleIdle()
{
    System().LogData(FLIGHTSTATE_IDLE);
    std::cout << "[STATE] IDLE -> ARMED" << std::endl;
    return FLIGHTSTATE_ARMED;
}

FlightState_t FlightState_HandleArmed()
{
    System().LogData(FLIGHTSTATE_ARMED);

    float accel = System().GetAccelZ();
    float alt = System().GetAltitude();

    if (accel > THRESHOLD_ACC && alt > THRESHOLD_Altitude)
    {
        std::cout << "[STATE] ARMED -> LAUNCH (Acc: " << accel << ", Alt: " << alt << ")" << std::endl;
        return FLIGHTSTATE_LAUNCH;
    }
    return FLIGHTSTATE_ARMED;
}

FlightState_t FlightState_HandleLaunch()
{
    System().LogData(FLIGHTSTATE_LAUNCH);
    std::cout << "[STATE] LAUNCH -> ASCENT" << std::endl;
    return FLIGHTSTATE_ASCENT;
}

FlightState_t FlightState_HandleAscent()
{
    System().LogData(FLIGHTSTATE_ASCENT);

    float accel = System().GetAccelZ();
    
    if (accel < 5.0f) 
    {
        std::cout << "[STATE] ASCENT -> CRUISING (Burnout detected)" << std::endl;
        return FLIGHTSTATE_CRUISING;
    }
    return FLIGHTSTATE_ASCENT;
}

FlightState_t FlightState_HandleCruising()
{
    System().LogData(FLIGHTSTATE_CRUISING);

    float velocity = System().GetVelocityZ();
    
    if (velocity < 0.0f)
    {
        std::cout << "[STATE] CRUISING -> APOGEE (Apogee detected)" << std::endl;
        return FLIGHTSTATE_APOGEE;
    }
    return FLIGHTSTATE_CRUISING;
}

FlightState_t FlightState_HandleApogee()
{
    System().LogData(FLIGHTSTATE_APOGEE);
    std::cout << "[STATE] APOGEE -> DEPLOYMENT" << std::endl;
    return FLIGHTSTATE_DEPLOYMENT;
}

FlightState_t FlightState_HandleDeployment()
{
    System().LogData(FLIGHTSTATE_DEPLOYMENT);
    std::cout << "[STATE] DEPLOYMENT -> DESCENT" << std::endl;
    return FLIGHTSTATE_DESCENT;
}

FlightState_t FlightState_HandleDescent()
{
    System().LogData(FLIGHTSTATE_DESCENT);

    float alt = System().GetAltitude();
    float velocity = System().GetVelocityZ();

    if (alt < 1.0f && std::abs(velocity) < 0.5f)
    {
        std::cout << "[STATE] DESCENT -> LANDED" << std::endl;
        return FLIGHTSTATE_LANDED;
    }
    return FLIGHTSTATE_DESCENT;
}

FlightState_t FlightState_HandleLanded()
{
    System().LogData(FLIGHTSTATE_LANDED);
    return FLIGHTSTATE_LANDED;
}

FlightState_t FlightState_HandleFailsafe()
{
    System().LogData(FLIGHTSTATE_FAILSAFE);
    return FLIGHTSTATE_FAILSAFE;
}

} // namespace Core
} // namespace ARK