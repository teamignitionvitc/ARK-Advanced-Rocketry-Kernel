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
* File:
* Author: Harsh
* Created On: 30/11/25
*
* \brief Header file for flight configuration.
* \details
*   - Defines the global Flight Data Configuration.
*   - Declares and sets the value of Thresholds called by the Flight States functions.
*
**************************************************************************************************/
#ifndef FLIGHT_CONFIG_H
#define FLIGHT_CONFIG_H
#define THRESHOLD_ACC       (15.0f)
#define THRESHOLD_Altitude  (1.0f)
#define THRESHold_APOGE     (1.0f)
#define TIME_LANDING_STABLE_MS      (5000U)
#define THRESH_LANDING_ALT_NOISE    (0.25f)
#endif /* flight_config.h */