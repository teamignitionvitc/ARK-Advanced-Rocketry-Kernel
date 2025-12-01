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
* Author: Harsh
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
* 003  NEW       30/11/25   Harsh   Logic correction,Inner functional implementation.
**************************************************************************************************/

#include "flight_states.h"
#include <cstdint>
#include "flight_system.h"
#include "flight_config.h"
#include "math.h"



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
    if (FlightSystem::CheckCommand("ARM"))
    {
        return FLIGHTSTATE_ARMED;
    }

    return FLIGHTSTATE_IDLE;
}
/*
 * need to create object of flight system to access non-static methods
 */
FlightState_t FlightState_HandleArmed()
{
    float currentAlt   = FlightSystem::GetAltitude();
    float currentAccel = FlightSystem::GetAccelZ();
    if ((currentAlt > THRESHOLD_Altitude) || (currentAccel > THRESHOLD_ACC))
    {
        return FLIGHTSTATE_LAUNCH;
    }
    return FLIGHTSTATE_ARMED;
}

FlightState_t FlightState_HandleLaunch()
{
    FlightSystem::LogData();
    return FLIGHTSTATE_ASCENT;
}

FlightState_t FlightState_HandleAscent()
{
    float currentA = FlightSystem::GetAccelZ();
    if (currentA < 0.0f)
    {
        return FLIGHTSTATE_CRUISING;
    }
    return FLIGHTSTATE_ASCENT;
}

FlightState_t FlightState_HandleCruising()
{
    float currentV = FlightSystem::GetVelocityZ();
    if (currentV <= 0.0f)
    {
        return FLIGHTSTATE_APOGEE;
    }
    return FLIGHTSTATE_CRUISING;
}

FlightState_t FlightState_HandleApogee()
{
    static float s_maxAltitude = 0.0f;
    float currentAlt = FlightSystem::GetAltitude();
    if (currentAlt > s_maxAltitude)
    {
        s_maxAltitude = currentAlt;
    }
    if ((s_maxAltitude - currentAlt) >= THRESHold_APOGE)
    {
        return FLIGHTSTATE_DEPLOYMENT;
    }

    return FLIGHTSTATE_DEPLOYMENT;
}

FlightState_t FlightState_HandleDeployment()
{
    FlightSystem::DeployParachute();
    return FLIGHTSTATE_DESCENT;
}

FlightState_t FlightState_HandleDescent()
{
    static uint32_t s_timerStart = 0;
    static float    s_lastAlt    = 0.0f;
    static bool     s_timerActive = false;
    float currentAlt = FlightSystem::GetAltitude();
    uint32_t now     = millis();
    if (fabs(currentAlt - s_lastAlt) < THRESH_LANDING_ALT_NOISE) {
        if (!s_timerActive)
        {
            s_timerStart = now;
            s_timerActive = true;
        }
        if ((now - s_timerStart) >= TIME_LANDING_STABLE_MS)
        {
            return FLIGHTSTATE_LANDED;
        }
    }else {
        s_timerActive = false;
    }
    s_lastAlt = currentAlt;
    return FLIGHTSTATE_DESCENT;
}
/*
 * buzzer beep function only when buzzer is installed.
*/
FlightState_t FlightState_HandleLanded()
{
   /* FlightSystem::Actuators_BuzzerBeep();*/
    return FLIGHTSTATE_LANDED;
}
FlightState_t FlightState_HandleFailsafe()
{
    FlightSystem::DeployParachute();
    return FLIGHTSTATE_FAILSAFE;
}