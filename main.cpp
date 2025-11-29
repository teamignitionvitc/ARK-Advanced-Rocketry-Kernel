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
* 001  NEW       23/11/25   Anubhav   Initial creation
* 002  NEW       30/11/25   Kunsh Jain   Initial creation,Fixed Structure
**************************************************************************************************/

#include "main.h"

/**************************************************************************************************
* Static Variables
**************************************************************************************************/

static FlightState_t gCurrentFlightState = FLIGHTSTATE_BOOT;

/**************************************************************************************************
* Function Definitions
**************************************************************************************************/

int main(void)
{
    FlightSystem system;

    /* Initialize system */
    system.System_Init();

    /* Main loop */
    while (true)
    {
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
    }
    return 0;
}
