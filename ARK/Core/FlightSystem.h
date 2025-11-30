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
* 001  NEW       23/11/25   Anubhav          Initial creation
* 002  NEW       30/11/25   Kunsh Jain       Initial creation,Fixed Structure
* 003  MOD       30/11/25   MuhammadRamzy    Refactored to ARK Framework namespace
**************************************************************************************************/

#ifndef ARK_CORE_FLIGHT_SYSTEM_H
#define ARK_CORE_FLIGHT_SYSTEM_H

#include "Config.h"
#include "../HAL/ISensor.h"
#include <memory>

namespace ARK {
namespace Core {

/**
 * @class FlightSystem
 * @brief Main system class for managing sensors, actuators, and telemetry.
 * @details The FlightSystem follows the Singleton pattern and serves as the central
 *          hub for all hardware interactions. It manages sensor instances via
 *          dependency injection and provides a unified interface for the state machine.
 */
class FlightSystem
{
public:
    /**
     * @brief Constructor.
     */
    FlightSystem();

    /**
     * @brief Destructor.
     */
    ~FlightSystem();

    /**
     * @brief Get the singleton instance of the FlightSystem.
     * @return Reference to the FlightSystem instance.
     */
    static FlightSystem& GetInstance();

    /**
     * @brief Initialize all registered system components.
     * @details Calls Init() on all registered sensors and storage devices.
     */
    void System_Init();
    
    /**
     * @brief Log current telemetry data to storage.
     * @param state The current flight state to include in the log.
     */
    void LogData(int state);
    
    /**
     * @brief Get the current altitude.
     * @return Altitude in meters.
     */
    float GetAltitude();

    /**
     * @brief Get the current Z-axis acceleration.
     * @return Acceleration in m/s^2.
     */
    float GetAccelZ();

    /**
     * @brief Get the current vertical velocity.
     * @return Velocity in m/s.
     */
    float GetVelocityZ();
    
    /**
     * @brief Check the health of the system.
     * @return true if all critical components are healthy, false otherwise.
     */
    static bool CheckHealth();

    /**
     * @brief Register an altimeter sensor.
     * @param sensor Pointer to the ISensor implementation. Ownership is transferred to FlightSystem.
     */
    void RegisterAltimeter(ARK::HAL::ISensor* sensor);

    /**
     * @brief Register an accelerometer sensor.
     * @param sensor Pointer to the ISensor implementation. Ownership is transferred to FlightSystem.
     */
    void RegisterAccelerometer(ARK::HAL::ISensor* sensor);

    /**
     * @brief Register a storage device.
     * @param storage Pointer to the IStorage implementation. Ownership is transferred to FlightSystem.
     */
    void RegisterStorage(ARK::HAL::IStorage* storage);

    /**
     * @brief Get the registered altimeter instance (for testing/simulation).
     * @return Pointer to the altimeter ISensor.
     */
    ARK::HAL::ISensor* GetAltimeter() { return m_altimeter.get(); }

    /**
     * @brief Get the registered accelerometer instance (for testing/simulation).
     * @return Pointer to the accelerometer ISensor.
     */
    ARK::HAL::ISensor* GetAccelerometer() { return m_accelerometer.get(); }

private:
    /**
     * @brief Internal method to read all sensors.
     */
    void ReadSensors();

    /**
     * @brief Internal method to control actuators (e.g., parachutes).
     */
    void ControlActuators();

    // HAL Components
    std::unique_ptr<ARK::HAL::ISensor> m_altimeter;      /**< Altimeter sensor instance */
    std::unique_ptr<ARK::HAL::ISensor> m_accelerometer;  /**< Accelerometer sensor instance */
    std::unique_ptr<ARK::HAL::IStorage> m_storage;       /**< Storage device instance */

    // Internal state
    float m_currentAltitude;   /**< Current altitude reading */
    float m_currentAccelZ;     /**< Current acceleration reading */
    float m_currentVelocityZ;  /**< Calculated vertical velocity */
    float m_lastAltitude;      /**< Previous altitude reading for velocity calc */
    uint32_t m_lastUpdate;     /**< Timestamp of last update */
};

} // namespace Core
} // namespace ARK

#endif // ARK_CORE_FLIGHT_SYSTEM_H
