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
* Author: Anubhav, Kunsh Jain
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
**************************************************************************************************/

#ifndef FLIGHT_SYSTEM_H
#define FLIGHT_SYSTEM_H

#include "config.h"

/**
 * @class FlightSystem
 * @brief Main system class for managing sensors, actuators, and telemetry
 * @details The FlightSystem class provides a unified interface for all hardware interactions
 *          including sensor data acquisition, actuator control, and data logging.
 *          It abstracts hardware-specific details and provides a clean API for the
 *          flight state machine.
 * 
 * @note This class should be instantiated once in main() and used throughout the flight
 */
class FlightSystem
{
public:
    /**
     * @brief Constructor for FlightSystem
     * @details Initializes the FlightSystem object. Actual hardware initialization
     *          is performed in System_Init().
     */
    FlightSystem();
    
    /**
     * @brief Destructor for FlightSystem
     * @details Cleans up resources and performs shutdown procedures
     */
    ~FlightSystem();

    /**
     * @brief Initialize all system components
     * @details Initializes sensors, actuators, communication interfaces, and data logging.
     *          This function must be called before any other FlightSystem methods.
     * @note Should be called once during BOOT state
     * @see FlightState_HandleBoot()
     */
    void System_Init();
    
    /**
     * @brief Log current telemetry data
     * @details Records current sensor readings and flight state to persistent storage.
     *          Typically called periodically during flight.
     * @note Implementation should be non-blocking for real-time performance
     */
    void LogData();
    
    /**
     * @brief Get current altitude reading
     * @details Retrieves the current altitude above ground level from the barometric sensor.
     * @return float Current altitude in meters (AGL - Above Ground Level)
     * @note Returns 0.0f if sensor not initialized or reading failed
     */
    float GetAltitude();
    
    /**
     * @brief Get current Z-axis acceleration
     * @details Retrieves the current vertical acceleration from the accelerometer.
     *          Positive values indicate upward acceleration.
     * @return float Current Z-axis acceleration in m/s²
     * @note Returns 0.0f if sensor not initialized or reading failed
     * @note At rest on ground, should read approximately 9.8 m/s² (gravity)
     */
    float GetAccelZ();
    
    /**
     * @brief Get current vertical velocity
     * @details Retrieves the current vertical velocity, typically derived from
     *          sensor fusion or integration of acceleration data.
     * @return float Current vertical velocity in m/s (positive = ascending)
     * @note Returns 0.0f if sensor not initialized or calculation failed
     */
    float GetVelocityZ();
    
    /**
     * @brief Check overall system health status
     * @details Performs health checks on all critical systems including sensors,
     *          power supply, and communication interfaces.
     * @return bool true if all systems are healthy, false if any system has failed
     * @note Static method - can be called without FlightSystem instance
     * @see FlightState_HandleBoot()
     */
    static bool CheckHealth();  // Static method

private:
    /**
     * @brief Read data from all sensors
     * @details Internal method to acquire data from all connected sensors.
     *          Called periodically by the main loop.
     * @note This is a private method - use public getter methods to access sensor data
     */
    void ReadSensors();
    
    /**
     * @brief Control actuators based on current state
     * @details Internal method to manage actuator outputs (e.g., parachute deployment,
     *          ignition systems) based on the current flight state.
     * @note This is a private method - actuators are controlled automatically
     */
    void ControlActuators();
};

#endif // FLIGHT_SYSTEM_H
