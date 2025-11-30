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

/**
 * @defgroup LaunchDetection Launch Detection Configuration
 * @brief Thresholds and parameters for detecting rocket liftoff
 * @{
 */

/**
 * @brief Acceleration threshold for launch detection
 * @details Minimum Z-axis acceleration (in m/s²) required to trigger launch detection.
 *          Set to approximately 50-70% of expected peak motor acceleration.
 *          Default value represents ~1.5G acceleration.
 * @note Adjust based on motor specifications:
 *       - Low-power motors: 10-15 m/s²
 *       - Mid-power motors: 15-25 m/s²
 *       - High-power motors: 25+ m/s²
 */
#define THRESHOLD_ACC    (15.0f)   /* ~1.5 Gs */

/**
 * @brief Minimum altitude threshold for launch confirmation
 * @details Minimum altitude (in meters) above ground level required to confirm liftoff.
 *          Prevents false triggers from ground vibrations or handling.
 * @note Typical range: 0.5-2.0 meters
 */
#define THRESHOLD_Altitude         (1.0f)    /* 1 Meters */

/** @} */ // end of LaunchDetection group

/**
 * @defgroup ApogeeDetection Apogee Detection Configuration
 * @brief Parameters for detecting peak altitude (apogee)
 * @{
 */

/**
 * @brief Altitude drop threshold for apogee confirmation
 * @details Minimum altitude decrease (in meters) required to confirm apogee has been reached.
 *          Larger values provide more reliable detection but may delay confirmation.
 * @note Recommended values:
 *       - Smaller rockets: 0.5-1.0 m
 *       - Larger rockets: 1.0-2.0 m
 */
#define THRESHOLD_APOGEE        (1.0f)    /* Distance to drop before confirming apogee */

/** @} */ // end of ApogeeDetection group

/**
 * @defgroup LandingDetection Landing Detection Configuration
 * @brief Parameters for detecting touchdown and landing confirmation
 * @{
 */

/**
 * @brief Time duration for landing stability check
 * @details Duration (in milliseconds) that altitude must remain stable to confirm landing.
 *          Prevents false positives during descent turbulence or bouncing.
 * @note Default: 5000 ms (5 seconds)
 */
#define TIME_LANDING_STABLE_MS      (5000U)   /* 5 Seconds of stability */

/**
 * @brief Altitude noise threshold for landing detection
 * @details Maximum acceptable altitude variation (in meters) when checking for stable landing.
 *          Accounts for barometric sensor noise and minor environmental fluctuations.
 * @note Typical sensor noise: ±0.1 to ±0.5 meters
 */
#define THRESH_LANDING_ALT_NOISE    (0.25f)    /* +/- 0.25m allowed noise */

/** @} */ // end of LandingDetection group

/**
 * @defgroup SystemTiming System Timing Configuration
 * @brief Main loop timing and execution parameters
 * @{
 */

/**
 * @brief Main control loop period
 * @details Period (in milliseconds) of the main state machine loop.
 *          Determines the update rate for sensor readings and state transitions.
 *          Default: 10 ms = 100 Hz update rate
 * @note Lower values = faster response but higher CPU usage
 *       Typical range: 5-20 ms (200-50 Hz)
 */
#define MAIN_LOOP_PERIOD_MS   (10U)   /* 100 Hz main loop (non-blocking) */

/** @} */ // end of SystemTiming group

#endif // CONFIG_H
