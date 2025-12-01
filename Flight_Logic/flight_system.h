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
* File: flight_system.h
* Author: Harsh
* Created On: 30/11/25
*
* \brief Header for FlightSystem class that manages sensors and actuators.
* \details
*   - Defines the FlightSystem class with its methods for sensor readings and actuator control.
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

#ifndef FLIGHT_SYSTEM_H
#define FLIGHT_SYSTEM_H

#include <bits/basic_string.h>
#include <bits/char_traits.h>
#include <bits/stringfwd.h>

#include "config.h"

class FlightSystem
{
public:
    FlightSystem();
    ~FlightSystem();

    void System_Init();
    void LogData();
    float GetAltitude();
    float GetAccelZ();
    float GetVelocityZ();
    static bool CheckHealth();  // Static method
    bool CheckCommand(std::string CommandByGS);
    void DeployParachute();

private:
    // Sensor and Actuator control functions
    void ReadSensors();
    void ControlActuators();
};

#endif // FLIGHT_SYSTEM_H
