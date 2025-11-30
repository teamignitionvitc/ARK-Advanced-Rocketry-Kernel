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

/**
 * @brief Global variable tracking the current flight state
 * @details This static variable maintains the current state of the flight state machine.
 *          It is initialized to FLIGHTSTATE_BOOT and transitions through various states
 *          during the flight lifecycle.
 * @note This variable is modified only by state handler functions
 */
static FlightState_t gCurrentFlightState = FLIGHTSTATE_BOOT;

/**************************************************************************************************
* Function Definitions
**************************************************************************************************/

/**
 * @brief Main entry point for the ARK flight computer firmware
 * @details Initializes the flight system and runs the main state machine loop.
 *          The loop continuously monitors the current flight state and dispatches
 *          to the appropriate state handler function.
 * 
 * @return int Returns 0 on normal termination (never reached in embedded systems)
 * 
 * @note This function runs indefinitely in an infinite loop.
 *       State transitions are handled by individual state handler functions.
 * 
 * @see FlightSystem::System_Init()
 * @see FlightState_HandleBoot()
 * @see FlightState_HandleIdle()
 * @see FlightState_HandleArmed()
 * @see FlightState_HandleLaunch()
 * @see FlightState_HandleAscent()
 * @see FlightState_HandleCruising()
 * @see FlightState_HandleApogee()
 * @see FlightState_HandleDeployment()
 * @see FlightState_HandleDescent()
 * @see FlightState_HandleLanded()
 * @see FlightState_HandleFailsafe()
 */
int main(void)
{
    FlightSystem system;

    /* Initialize system */
    system.System_Init();

    /**
     * @brief Main state machine loop
     * @details Infinite loop that continuously:
     *          1. Checks the current flight state
     *          2. Calls the appropriate state handler
     *          3. Updates the current state based on handler return value
     *          4. Repeats at MAIN_LOOP_PERIOD_MS frequency
     */
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
