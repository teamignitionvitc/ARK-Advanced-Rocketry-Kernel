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
* File: config.h
* Author: Kunsh Jain
* Created On: 30/11/25
*
* \brief Global macros and constants for the A-R-K Flight Computer firmware.
* \details
*   - Defines all global constants that can be used across all modules.
*
***************************************************************************************************
* HISTORY:
*
* +----- (NEW | MODify | ADD | DELete)
* |
* No#  | when      who        what
******+*********+***********+**********************************************************************
* 001  NEW       30/11/25   Kunsh Jain   Initial creation
**************************************************************************************************/

#ifndef CONFIG_H
#define CONFIG_H

/* Launch Detection Thresholds */
#define THRESHOLD_ACC    (15.0f)   /* ~1.5 Gs */
#define THRESHOLD_Altitude         (1.0f)    /* 1 Meters */

/* Apogee Detection */
#define THRESHOLD_APOGEE        (1.0f)    /* Distance to drop before confirming apogee */

/* Landing Detection */
#define TIME_LANDING_STABLE_MS      (5000U)   /* 5 Seconds of stability */
#define THRESH_LANDING_ALT_NOISE    (0.25f)    /* +/- 0.25m allowed noise */

/* Main Loop Period */
#define MAIN_LOOP_PERIOD_MS   (10U)   /* 100 Hz main loop (non-blocking) */

#endif // CONFIG_H
