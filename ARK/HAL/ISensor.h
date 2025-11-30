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
* File: ISensor.h
* Author: Kunsh Jain
* Created On: 30/11/25
*
* \brief Hardware Abstraction Layer (HAL) interfaces.
* \details
*   - Defines abstract base classes for Sensors and Storage.
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

#ifndef ARK_HAL_ISENSOR_H
#define ARK_HAL_ISENSOR_H

#include <cstdint>
#include <string>

namespace ARK {
namespace HAL {

/**
 * @class ISensor
 * @brief Abstract base class for all sensor implementations.
 * @details This interface defines the standard methods that all sensors must implement
 *          to be compatible with the ARK FlightSystem.
 */
class ISensor
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~ISensor() {}

    /**
     * @brief Initialize the sensor hardware.
     * @return true if initialization was successful, false otherwise.
     */
    virtual bool Init() = 0;

    /**
     * @brief Read the current value from the sensor.
     * @return float The latest sensor reading (unit depends on sensor type).
     */
    virtual float Read() = 0;
    
    /**
     * @brief Get the human-readable name of the sensor.
     * @return std::string Name of the sensor.
     */
    virtual std::string GetName() const = 0;
};

/**
 * @class IStorage
 * @brief Abstract base class for storage device implementations.
 * @details This interface defines the standard methods for logging data to
 *          storage media (SD card, Flash, etc.).
 */
class IStorage
{
public:
    /**
     * @brief Virtual destructor.
     */
    virtual ~IStorage() {}

    /**
     * @brief Initialize the storage hardware.
     * @return true if initialization was successful, false otherwise.
     */
    virtual bool Init() = 0;

    /**
     * @brief Write a string of data to the storage medium.
     * @param data The string content to write.
     * @return true if write was successful, false otherwise.
     */
    virtual bool Write(const std::string& data) = 0;
};

} // namespace HAL
} // namespace ARK

#endif // ARK_HAL_ISENSOR_H
