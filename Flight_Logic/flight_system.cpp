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
* File: flight_system.cpp
* Author: Harsh
* Created On: 30/11/25
*
* \brief FlightSystem class implementation for managing sensors and actuators.
* \details
*   - Implements the FlightSystem methods for reading sensor data and logging information.
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
* 003  ADD       30/11/25   Harsh     CheckCommand,DeployParachute
**************************************************************************************************/

#include "flight_system.h"
#include <iostream>


FlightSystem::FlightSystem() {}
FlightSystem::~FlightSystem() {}

void FlightSystem::System_Init()
{
    // Initialize sensors and actuators
}

void FlightSystem::LogData()
{
    // Log flight data (Telemetry)
    std::cout << "Logging data..." << std::endl;
}

float FlightSystem::GetAltitude()
{
    // Get altitude reading
    return 0.0f;  // Placeholder value
}

float FlightSystem::GetAccelZ()
{
    // Get Z-axis acceleration
    return 0.0f;  // Placeholder value
}

float FlightSystem::GetVelocityZ()
{
    // Get Z-axis velocity
    return 0.0f;  // Placeholder value
}

bool FlightSystem::CheckHealth()
{
    // Check if all sensors and systems are healthy
    return true;  // Placeholder value
}
bool FlightSystem::CheckCommand(std::string CommandByGS)
{
    //continouisly check for the comamnd from the ground station to arm the rocket
    return true;
}
void FlightSystem::DeployParachute()
{
    // activate the acheutators when called
    FlightSystem::ControlActuators();

}
