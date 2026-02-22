/**
 * @file ark.h
 * @brief ARK Framework - Unified API Header
 * 
 * Single header for all ARK functionality. Include this in your rocket code:
 * 
 *     #include <ark.h>
 * 
 * Example usage:
 *     void setup() {
 *         ark.begin();
 *         ark.buzzer.beep(2);
 *     }
 * 
 *     void loop() {
 *         if (ark.state == FLIGHT) {
 *             if (ark.velocity() < -2) {
 *                 ark.deployDrogue();
 *             }
 *         }
 *     }
 * 
 * @copyright Team Ignition VIT
 */

#ifndef ARK_H
#define ARK_H

#include "../Config.hpp"
#include "Kernel/StateMachine/States.hpp"
#include "HAL/Console/Console.hpp"
#include "HAL/Time/Time.hpp"

// Include enabled modules
#if ENABLE_BMP280
#include "../Modules/BMP280/BMP280.hpp"
#endif

#if ENABLE_MPU9250
#include "../Modules/MPU9250/MPU9250.hpp"
#endif

#if ENABLE_BUZZER
#include "../Modules/Buzzer/Buzzer.hpp"
#endif

#if ENABLE_LED
#include "../Modules/LED/LED.hpp"
#endif

#include "../Modules/ModulesManager.hpp"

/**
 * @class Ark
 * @brief Unified API for ARK framework
 * 
 * Provides Arduino-like simplicity for accessing sensors, state machine,
 * and recovery systems. All functions use natural units (meters, m/s, etc.)
 */
class Ark {
public:
    // =========================================================================
    // Initialization
    // =========================================================================
    
    /**
     * @brief Initialize the ARK system
     * Call this once in setup()
     */
    void begin();
    
    /**
     * @brief Main update tick
     * Call this at the start of loop() for automatic state management
     */
    void update();
    
    // =========================================================================
    // State Machine (read-only access to current flight state)
    // =========================================================================
    
    /** Current flight state (IDLE, ARMED, FLIGHT, etc.) */
    FlightState state;
    
    /** @brief Check if system is armed */
    bool isArmed() const { return FlightManager.isArmed(); }
    
    /** @brief Check if currently in flight (any flying state) */
    bool isFlying() const { return FlightManager.isFlying(); }
    
    /** @brief Check if landed */
    bool isLanded() const { return FlightManager.isLanded(); }
    
    /** @brief Get state name as string */
    const char* stateName() const { return FlightManager.GetStateName(); }
    
    // =========================================================================
    // State Control
    // =========================================================================
    
    /** @brief Arm the system (transition to ARMED state) */
    void arm();
    
    /** @brief Disarm the system (transition back to IDLE) */
    void disarm();
    
    // =========================================================================
    // Sensor Data (unified interface)
    // =========================================================================
    
#if ENABLE_BMP280
    /** @brief Get current altitude in meters (relative to ground) */
    float altitude();
    
    /** @brief Get barometric pressure in hPa */
    float pressure();
    
    /** @brief Get temperature in Celsius */
    float temperature();
    
    /** @brief Get maximum altitude reached during flight */
    float maxAltitude();
    
    /** @brief Calibrate ground level (call before arming) */
    void calibrateAltimeter();
#endif

#if ENABLE_MPU9250
    /** @brief Get acceleration magnitude in m/s² */
    float acceleration();
    
    /** @brief Get X-axis acceleration in m/s² */
    float accelX();
    
    /** @brief Get Y-axis acceleration in m/s² */
    float accelY();
    
    /** @brief Get Z-axis acceleration in m/s² */
    float accelZ();
    
    /** @brief Get X-axis angular rate in deg/s */
    float gyroX();
    
    /** @brief Get Y-axis angular rate in deg/s */
    float gyroY();
    
    /** @brief Get Z-axis angular rate in deg/s */
    float gyroZ();
#endif

    /** @brief Get estimated vertical velocity in m/s (+ up, - down) */
    float velocity();
    
    // =========================================================================
    // Recovery System
    // =========================================================================
    
    /** @brief Fire drogue deployment pyro channel */
    void deployDrogue();
    
    /** @brief Fire main deployment pyro channel */
    void deployMain();
    
    /** @brief Check if drogue has been deployed */
    bool isDrogueDeployed() const { return _drogueDeployed; }
    
    /** @brief Check if main has been deployed */
    bool isMainDeployed() const { return _mainDeployed; }
    
    // =========================================================================
    // Output Devices (nested for clean syntax: ark.led.on())
    // =========================================================================
    
#if ENABLE_LED
    struct LED_API {
        void on();
        void off();
        void toggle();
        void blink(uint32_t interval_ms);
    } led;
#endif

#if ENABLE_BUZZER
    struct Buzzer_API {
        void beep(uint8_t count = 1);
        void tone(uint16_t frequency_hz, uint32_t duration_ms = 0);
        void stop();
    } buzzer;
#endif

    // =========================================================================
    // Logging
    // =========================================================================
    
    /** @brief Log a message to console/storage */
    void log(const char* message);
    
    /** @brief Log a formatted message */
    void logf(const char* format, ...);
    
    // =========================================================================
    // System Info
    // =========================================================================
    
    /** @brief Get system uptime in milliseconds */
    uint32_t uptime() const;
    
    /** @brief Get time spent in current state (ms) */
    uint32_t stateTime() const;
    
private:
    float _maxAltitude = 0.0f;
    float _groundAltitude = 0.0f;
    float _lastAltitude = 0.0f;
    float _velocity = 0.0f;
    uint32_t _lastVelocityUpdate = 0;
    
    bool _drogueDeployed = false;
    bool _mainDeployed = false;
    
    uint32_t _startTime = 0;
    
    void updateState();
    void updateSensors();
    void updateVelocity();
};

// Global Ark instance - use this in your code
extern Ark ark;

// =========================================================================
// Arduino-style Entry Points (user implements these)
// =========================================================================

/**
 * @brief User setup function - called once at startup
 * 
 * Example:
 *     void setup() {
 *         ark.begin();
 *         ark.buzzer.beep(2);  // Ready signal
 *     }
 */
void setup();

/**
 * @brief User loop function - called repeatedly
 * 
 * Example:
 *     void loop() {
 *         if (ark.state == FLIGHT && ark.velocity() < -2) {
 *             ark.deployDrogue();
 *         }
 *     }
 */
void loop();

#endif // ARK_H
