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
* Author: Kunsh Jain
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
* 001  NEW       23/11/25   Anubhav          Initial creation
* 002  NEW       30/11/25   Kunsh Jain       Initial creation,Fixed Structure
* 003  MOD       30/11/25   MuhammadRamzy    Refactored to ARK Framework namespace
**************************************************************************************************/

#include "FlightSystem.h"
#include <iostream>
#include <chrono>
#include <cmath>

namespace ARK {
namespace Core {

// Singleton instance
static FlightSystem* s_instance = nullptr;

/**
 * @brief Constructor. Initializes internal state variables.
 */
FlightSystem::FlightSystem() 
    : m_currentAltitude(0.0f)
    , m_currentAccelZ(0.0f)
    , m_currentVelocityZ(0.0f)
    , m_lastAltitude(0.0f)
    , m_lastUpdate(0)
{
    s_instance = this;
}

/**
 * @brief Destructor. Clears the singleton instance.
 */
FlightSystem::~FlightSystem() 
{
    s_instance = nullptr;
}

/**
 * @brief Get the singleton instance.
 * @return Reference to the FlightSystem instance.
 */
FlightSystem& FlightSystem::GetInstance()
{
    return *s_instance;
}

/**
 * @brief Register an altimeter sensor.
 * @param sensor Pointer to the sensor implementation.
 */
void FlightSystem::RegisterAltimeter(ARK::HAL::ISensor* sensor)
{
    m_altimeter.reset(sensor);
}

/**
 * @brief Register an accelerometer sensor.
 * @param sensor Pointer to the sensor implementation.
 */
void FlightSystem::RegisterAccelerometer(ARK::HAL::ISensor* sensor)
{
    m_accelerometer.reset(sensor);
}

/**
 * @brief Register a storage device.
 * @param storage Pointer to the storage implementation.
 */
void FlightSystem::RegisterStorage(ARK::HAL::IStorage* storage)
{
    m_storage.reset(storage);
}

/**
 * @brief Initialize the system and all registered hardware.
 */
void FlightSystem::System_Init()
{
    std::cout << "[FlightSystem] Initializing..." << std::endl;

    // Initialize Hardware
    if (m_altimeter) {
        if (m_altimeter->Init()) {
            std::cout << "[FlightSystem] Altimeter initialized." << std::endl;
        } else {
            std::cerr << "[FlightSystem] Altimeter failed!" << std::endl;
        }
    } else {
        std::cerr << "[FlightSystem] No Altimeter Registered!" << std::endl;
    }

    if (m_accelerometer) {
        if (m_accelerometer->Init()) {
            std::cout << "[FlightSystem] Accelerometer initialized." << std::endl;
        } else {
            std::cerr << "[FlightSystem] Accelerometer failed!" << std::endl;
        }
    } else {
        std::cerr << "[FlightSystem] No Accelerometer Registered!" << std::endl;
    }

    if (m_storage) {
        if (m_storage->Init()) {
            std::cout << "[FlightSystem] Storage initialized." << std::endl;
        } else {
            std::cerr << "[FlightSystem] Storage failed!" << std::endl;
        }
    }
}

/**
 * @brief Read data from all registered sensors.
 * @details Updates internal state variables with fresh sensor data.
 */
void FlightSystem::ReadSensors()
{
    if (m_altimeter) {
        float newAlt = m_altimeter->Read();
        
        // Simple velocity calculation: v = (alt - last_alt) / dt
        // Assuming fixed time step for now or just using difference
        // In real system, use actual time delta
        
        // For simulation simplicity, let's assume this is called every MAIN_LOOP_PERIOD_MS
        float dt = (float)MAIN_LOOP_PERIOD_MS / 1000.0f;
        
        if (dt > 0.0f) {
            m_currentVelocityZ = (newAlt - m_lastAltitude) / dt;
        }
        
        m_lastAltitude = m_currentAltitude;
        m_currentAltitude = newAlt;
    }

    if (m_accelerometer) {
        m_currentAccelZ = m_accelerometer->Read();
    }
}

/**
 * @brief Log telemetry data to the registered storage device.
 * @param state Current flight state to log.
 */
void FlightSystem::LogData(int state)
{
    if (m_storage) {
        // Format: Timestamp, State, Alt, Vel, Accel
        // Using simple string formatting for now
        std::string logEntry = "State: " + std::to_string(state) + 
                             ", Alt: " + std::to_string(m_currentAltitude) + 
                             ", Vel: " + std::to_string(m_currentVelocityZ) + 
                             ", Acc: " + std::to_string(m_currentAccelZ);
        m_storage->Write(logEntry);
    }
}

/**
 * @brief Get current altitude.
 * @return Altitude in meters.
 */
float FlightSystem::GetAltitude()
{
    ReadSensors(); // Ensure fresh data
    return m_currentAltitude;
}

/**
 * @brief Get current Z-axis acceleration.
 * @return Acceleration in m/s^2.
 */
float FlightSystem::GetAccelZ()
{
    ReadSensors(); // Ensure fresh data
    return m_currentAccelZ;
}

/**
 * @brief Get current vertical velocity.
 * @return Velocity in m/s.
 */
float FlightSystem::GetVelocityZ()
{
    return m_currentVelocityZ;
}

/**
 * @brief Check system health.
 * @return true if healthy, false otherwise.
 */
bool FlightSystem::CheckHealth()
{
    // In a real system, we would check sensor status flags
    // For now, if instance exists and sensors are allocated, we are good
    if (!s_instance) return false;
    if (!s_instance->m_altimeter) return false;
    if (!s_instance->m_accelerometer) return false;
    return true;
}

/**
 * @brief Control system actuators.
 */
void FlightSystem::ControlActuators()
{
    // Placeholder for parachute deployment logic
}

} // namespace Core
} // namespace ARK
