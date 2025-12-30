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
                                                                                                
                  © Copyright of Ignition Software
************************************************************************************************
* File:        ark.h
* Brief:
* Public interface for the ARK kernel.
* Description:
* Defines the ARK kernel class responsible for initialization,
* execution, and halt.
************************************************************************************************

#ifndef ARK_H
#define ARK_H

/*==============================================================================================
 * Includes
 *============================================================================================*/
#include <stdint.h>
#include <stdbool.h>

/*==============================================================================================
 * Enumerations
 *=============================================================================================/
typedef enum
{
    RESULT_OK = 0,
    RESULT_INVALID_PARAM,
    RESULT_NOT_INITIALIZED,
    RESULT_ALREADY_INITIALIZED,
    RESULT_INTERNAL_ERROR
} ResultCode;

/*==============================================================================================
 * Typedefs / Structures
 *============================================================================================*/
typedef struct
{
    uint32_t expectedAltitudeMeters;
    // TODO: fill other flight configurations
} ArkConfig;

/*==============================================================================================
 * Class Declaration
 *============================================================================================*/
class ARK
{
public:
    /**
     * \brief   Initializes the ARK kernel.
     * \details Validates configuration and prepares internal kernel state.
     *
     * \param[in] pConfig Pointer to kernel configuration.
     * \return    RESULT_OK if successful.
     *
     * \pre       Kernel must not be initialized.
     * \post      Kernel enters initialized state.
     */
    ResultCode Init(const ArkConfig* pConfig);

    /**
     * \brief   Starts kernel-controlled execution.
     * \details Enters execution loop owned by ARK's StateManager.
     *
     * \return    RESULT_OK if execution begins successfully.
     *
     * \pre       Init() must have completed successfully.
     * \post      Kernel is running.
     */
    ResultCode Run(void);

    /**
     * \brief   Halts kernel execution safely.
     * \details Stops execution and transitions kernel to a safe state.
     *
     * \return    RESULT_OK if halt is successful.
     *
     * \pre       Kernel must be initialized.
     * \post      Kernel execution is halted.
     */
    ResultCode Halt(void);

private:
    bool mIsInitialized;
    bool mIsRunning;
    ArkConfig mConfig;

    /**
     * \brief   Validates ARK configuration.
     *
     * \param[in] pConfig Pointer to configuration to validate.
     * \return    true if configuration is valid, false otherwise.
     */
    bool ValidateConfig(const ArkConfig* pConfig);
};

#endif /* ARK_H */
