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
* File: MockSensors.h
* Author: Kunsh Jain
* Created On: 30/11/25
*
* \brief Mock implementations of HAL interfaces for simulation.
* \details
*   - Provides MockBarometer and MockAccelerometer.
*
***************************************************************************************************
* HISTORY:
*
* +----- (NEW | MODify | ADD | DELete)
* |
* No#  | when      who            what
* ******+*********+**************+*****************************************************************
* 001  NEW       30/11/25       Kunsh Jain       Initial creation
* 002  MOD       30/11/25       MuhammadRamzy    Refactored to ARK Framework namespace
**************************************************************************************************/

#ifndef ARK_HAL_MOCK_SENSORS_H
#define ARK_HAL_MOCK_SENSORS_H

#include "ISensor.h"
#include <iostream>

namespace ARK {
namespace HAL {

/**
 * @class MockBarometer
 * @brief Mock implementation of a barometer for simulation.
 * @implements ISensor
 */
class MockBarometer : public ISensor
{
private:
    float m_altitude;

public:
    /**
     * @brief Constructor. Initializes altitude to 0.
     */
    MockBarometer() : m_altitude(0.0f) {}

    /**
     * @brief Initialize the mock barometer.
     * @return Always returns true.
     */
    bool Init() override
    {
        std::cout << "[MockBarometer] Initialized" << std::endl;
        return true;
    }

    /**
     * @brief Read the simulated altitude.
     * @return float Current altitude in meters.
     */
    float Read() override
    {
        return m_altitude;
    }

    /**
     * @brief Get the sensor name.
     * @return "MockBarometer"
     */
    std::string GetName() const override
    {
        return "MockBarometer";
    }

    /**
     * @brief Set the simulated altitude.
     * @param alt Altitude in meters.
     */
    void SetAltitude(float alt)
    {
        m_altitude = alt;
    }
};

/**
 * @class MockAccelerometer
 * @brief Mock implementation of an accelerometer for simulation.
 * @implements ISensor
 */
class MockAccelerometer : public ISensor
{
private:
    float m_accelZ;

public:
    /**
     * @brief Constructor. Initializes Z-acceleration to 9.8 m/s^2 (1G).
     */
    MockAccelerometer() : m_accelZ(9.8f) {} // Default to 1G

    /**
     * @brief Initialize the mock accelerometer.
     * @return Always returns true.
     */
    bool Init() override
    {
        std::cout << "[MockAccelerometer] Initialized" << std::endl;
        return true;
    }

    /**
     * @brief Read the simulated Z-axis acceleration.
     * @return float Acceleration in m/s^2.
     */
    float Read() override
    {
        return m_accelZ;
    }

    /**
     * @brief Get the sensor name.
     * @return "MockAccelerometer"
     */
    std::string GetName() const override
    {
        return "MockAccelerometer";
    }

    /**
     * @brief Set the simulated Z-axis acceleration.
     * @param accel Acceleration in m/s^2.
     */
    void SetAccelZ(float accel)
    {
        m_accelZ = accel;
    }
};

/**
 * @class MockStorage
 * @brief Mock implementation of storage for simulation.
 * @implements IStorage
 */
class MockStorage : public IStorage
{
public:
    /**
     * @brief Initialize the mock storage.
     * @return Always returns true.
     */
    bool Init() override
    {
        std::cout << "[MockStorage] Initialized" << std::endl;
        return true;
    }

    /**
     * @brief Write data to the console (simulating storage write).
     * @param data The data string to write.
     * @return Always returns true.
     */
    bool Write(const std::string& data) override
    {
        std::cout << "[LOG] " << data << std::endl;
        return true;
    }
};

} // namespace HAL
} // namespace ARK

#endif // ARK_HAL_MOCK_SENSORS_H
