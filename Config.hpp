#pragma once

/* =============================================================================
 * ARK ROCKET CONFIGURATION
 * =============================================================================
 * 
 * Edit this file to configure your rocket. This is the ONLY config file!
 * 
 * QUICK START:
 * 1. Enable your sensors below (set to 1)
 * 2. Set your pin numbers
 * 3. Choose SIMPLE or ADVANCED flight mode
 * 4. Edit User/UserCode.cpp for your flight logic
 * 
 * =============================================================================
 */

/* =============================================================================
 * FLIGHT MODE
 * 
 * SIMPLE (recommended): 6 states - IDLE, ARMED, FLIGHT, RECOVERY, LANDED, FAULT
 *   Best for: Most rockets, dual-deploy, beginners
 * 
 * ADVANCED: 10 states with finer control
 *   Best for: Staging rockets, research, complex missions
 * =============================================================================
 */
#define ARK_FLIGHT_MODE_SIMPLE   0
#define ARK_FLIGHT_MODE_ADVANCED 1
#define ARK_FLIGHT_MODE          ARK_FLIGHT_MODE_SIMPLE

/* =============================================================================
 * SENSORS & MODULES
 * 
 * Set to 1 to enable, 0 to disable
 * Disabled modules use ZERO RAM!
 * =============================================================================
 */
#define ENABLE_BMP280    1    // Barometer (altitude, pressure, temperature)
#define ENABLE_MPU9250   1    // IMU (accelerometer + gyroscope)
#define ENABLE_QMC5883L  0    // Magnetometer (compass)
#define ENABLE_L89       0    // GPS
#define ENABLE_NRF24     0    // Radio transceiver
#define ENABLE_BUZZER    1    // Buzzer for audio feedback
#define ENABLE_LED       1    // Status LED
#define ENABLE_WS2812    0    // NeoPixel RGB LED

/* =============================================================================
 * PIN CONFIGURATION
 * =============================================================================
 */

// I2C Bus (for BMP280, MPU9250, QMC5883L)
#define I2C_SDA_PIN      2
#define I2C_SCL_PIN      3
#define I2C_FREQUENCY    100000

// Output Pins
#define PIN_LED          25   // Status LED (Pico onboard = 25)
#define PIN_BUZZER       6    // Buzzer

// Recovery System Pyro Channels
#define PIN_PYRO_DROGUE  10   // Drogue chute deployment
#define PIN_PYRO_MAIN    11   // Main chute deployment

/* =============================================================================
 * SYSTEM SETTINGS (usually no need to change)
 * =============================================================================
 */
#define MCU_FREQUENCY_MHZ       133
#define SYSTEM_FREQUENCY_HZ     200
#define MODULES_KERNEL_FREQUENCY 100
#define SYSTEM_KERNEL_FREQUENCY  150
#define USER_KERNEL_FREQUENCY    100
#define KERNEL_GUARD_US         1000

// Auto-computed (don't edit)
#define ENABLE_I2C   (ENABLE_BMP280 || ENABLE_MPU9250 || ENABLE_QMC5883L)
#define ENABLE_SPI   (ENABLE_NRF24)
#define I2C_INSTANCE 0
#define SPI_INSTANCE 0

/* =============================================================================
 * SIMULATION (for testing without hardware)
 * =============================================================================
 */
#define SIMULATION_MODE_ACTIVE false
#define SIL_MODE_ACTIVE        false
#define HIL_MODE_ACTIVE        false
