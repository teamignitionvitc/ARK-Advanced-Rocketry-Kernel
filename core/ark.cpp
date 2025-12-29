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
* File:        ark.cpp
* Brief:
* Implementation of the ARK kernel.
* Description:
* Implements kernel lifecycle control including initialization,
* execution, and halt mechanisms.
************************************************************************************************

/*==============================================================================================
 * Includes
 *============================================================================================*/
#include "ark.h"

/*==============================================================================================
 * Function Definitions
 *============================================================================================*/

ResultCode ARK::Init(const ArkConfig* pConfig)
{
    if (mIsInitialized == true)
    {
        return RESULT_ALREADY_INITIALIZED;
    }

    if (ValidateConfig(pConfig) == false)
    {
        return RESULT_INVALID_PARAM;
    }

    mConfig = *pConfig;
    mIsInitialized = true;
    mIsRunning = false;

    return RESULT_OK;
}

ResultCode ARK::Run(void)
{
    if (mIsInitialized == false)
    {
        return RESULT_NOT_INITIALIZED;
    }

    if (mIsRunning == true)
    {
        return RESULT_INTERNAL_ERROR;
    }

    mIsRunning = true;

    // TODO: Hand over to StateManager here


    return RESULT_INTERNAL_ERROR;
}

ResultCode ARK::Halt(void)
{
    if (mIsInitialized == false)
    {
        return RESULT_NOT_INITIALIZED;
    }

    mIsRunning = false;

    return RESULT_OK;
}

/*==============================================================================================
 * Private Function Definitions
 *============================================================================================*/

bool ARK::ValidateConfig(const ArkConfig* pConfig)
{
    if (pConfig == NULL)
    {
        return false;
    }

    if (pConfig->expectedAltitudeMeters == 0U)
    {
        return false;
    }

    return true;
}

