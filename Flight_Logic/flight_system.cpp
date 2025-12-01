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

/**
 * @brief FlightSystem constructor implementation
 * @details Initializes the FlightSystem object. Hardware initialization
 *          is deferred to System_Init() for better control.
 */
FlightSystem::FlightSystem() {}

/**
 * @brief FlightSystem destructor implementation
 * @details Performs cleanup of resources and shutdown procedures.
 */

FlightSystem::~FlightSystem() {}
/**
 * @brief Initialize all flight system components
 * @details Performs initialization of:
 *          - Barometric altimeter (BMP280/MS5611)
 *          - Accelerometer/IMU (MPU6050/BNO055)
 *          - GPS module (if equipped)
 *          - Data logging system
 *          - Communication interfaces
 * @note This function should be called once during BOOT state before
 *       any other FlightSystem methods are used.
 * @todo Implement actual sensor initialization code
 */
void FlightSystem::System_Init()
{
    // Initialize sensors and actuators
    // TODO: Add sensor initialization code here
    // Example:
    // bmp.begin();
    // mpu.begin();
    // gps.begin(9600);
}
/**
 * @brief Log current telemetry data to storage
 * @details Records current flight data including:
 *          - Timestamp
 *          - Current flight state
 *          - Altitude (AGL)
 *          - Vertical velocity
 *          - Vertical acceleration
 *          - GPS coordinates (if available)
 * @note This function should be non-blocking to maintain real-time performance.
 *       Data is typically buffered and written periodically to SD card or flash.
 * @todo Implement actual data logging to persistent storage
 */
// Log flight data (Telemetry)
    std::cout << "Logging data..." << std::endl;
    // TODO: Implement actual logging
    // Example:
    // logger.write(timestamp, altitude, velocity, acceleration);
}

/**
 * @brief Get current altitude above ground level
 * @details Reads altitude from barometric pressure sensor and converts to
 *          meters above ground level (AGL). Ground level is typically calibrated
 *          during system initialization.
 * @return float Current altitude in meters (AGL)
 * @retval 0.0f Sensor not initialized or reading failed
 * @note Altitude is derived from barometric pressure using standard atmosphere model
 * @todo Implement actual barometric sensor reading
 */
float FlightSystem::GetAltitude()
{
    // Get altitude reading from barometric sensor
    // TODO: Replace with actual sensor code
    // Example:
    // return bmp.readAltitude(SEA_LEVEL_PRESSURE) - groundLevelAltitude;
    return 0.0f;  // Placeholder value
}

/**
 * @brief Get current vertical (Z-axis) acceleration
 * @details Reads Z-axis acceleration from the accelerometer/IMU.
 *          Positive values indicate upward acceleration.
 *          At rest on the ground, should read approximately 9.8 m/s² (1G due to gravity).
 * @return float Current Z-axis acceleration in m/s²
 * @retval 0.0f Sensor not initialized or reading failed
 * @note Ensure accelerometer is properly oriented with Z-axis vertical
 * @note May require calibration to account for sensor offset
 * @todo Implement actual accelerometer reading
 */
float FlightSystem::GetAccelZ()
{
    // Get Z-axis acceleration from accelerometer
    // TODO: Replace with actual sensor code
    // Example:
    // sensors_event_t event;
    // mpu.getAccelerometerSensor()->getEvent(&event);
    // return event.acceleration.z;
    return 0.0f;  // Placeholder value
}

/**
 * @brief Get current vertical velocity
 * @details Calculates vertical velocity through sensor fusion or integration.
 *          Typically derived from:
 *          - Integration of acceleration data
 *          - Differentiation of altitude data
 *          - Kalman filter fusion of both
 *          Positive values indicate upward motion.
 * @return float Current vertical velocity in m/s
 * @retval 0.0f Sensor not initialized or calculation failed
 * @note Velocity estimation may drift over time without proper filtering
 * @todo Implement velocity calculation using sensor fusion
 */
float FlightSystem::GetVelocityZ()
{
    // Get Z-axis velocity (derived from sensors)
    // TODO: Implement sensor fusion or integration
    // Example:
    // return kalmanFilter.getVelocity();
    // or
    // velocity += GetAccelZ() * dt;
    // return velocity;
    return 0.0f;  // Placeholder value
}

/**
 * @brief Perform system health check
 * @details Verifies that all critical systems are functioning properly:
 *          - Barometric altimeter responding
 *          - Accelerometer/IMU responding
 *          - GPS lock (if required)
 *          - Battery voltage adequate
 *          - Communication interfaces active
 *          - Data logging system ready
 * @return bool System health status
 * @retval true All systems healthy and ready for flight
 * @retval false One or more critical systems have failed
 * @note This is a static method and can be called without a FlightSystem instance
 * @note Called during BOOT state to verify system readiness
 * @todo Implement actual health checks for all subsystems
 */
bool FlightSystem::CheckHealth()
{
    // Check if all sensors and systems are healthy
    // TODO: Implement actual health checks
    // Example:
    // if (!bmp.begin()) return false;
    // if (!mpu.begin()) return false;
    // if (batteryVoltage < MIN_VOLTAGE) return false;
    return true;  // Placeholder - returns true for testing
}bool FlightSystem::CheckCommand(std::string CommandByGS)
{
    //continouisly check for the comamnd from the ground station to arm the rocket
    return true;
}
void FlightSystem::DeployParachute()
{
    // activate the acheutators when called
    FlightSystem::ControlActuators();

}
