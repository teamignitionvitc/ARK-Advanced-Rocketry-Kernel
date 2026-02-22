/**
 * ============================================================================
 * ARK - User Rocket Code
 * ============================================================================
 * 
 * This is your rocket's flight code. Edit this file to control your rocket!
 * 
 * QUICK START:
 * 1. Edit Config.hpp to enable your sensors (ENABLE_BMP280, ENABLE_MPU9250, etc.)
 * 2. Set your pin numbers in Config.hpp
 * 3. Write your flight logic in UserLoop() below
 * 4. Build and flash to your board
 * 
 * FLIGHT STATES (SIMPLE mode):
 *   IDLE     - System ready, waiting for arm command
 *   ARMED    - Armed, monitoring for launch
 *   FLIGHT   - Active flight (launch → apogee → recovery)
 *   RECOVERY - Descending under parachutes
 *   LANDED   - On ground, mission complete
 *   FAULT    - Error state
 * 
 * ============================================================================
 */

#include "ARK/ark.h"

// ============================================================================
// CONFIGURATION - Adjust these for your rocket
// ============================================================================

const float LAUNCH_ACCEL_G      = 3.0f;    // G-force to detect launch (1G = 9.8 m/s²)
const float APOGEE_VELOCITY     = -2.0f;   // Negative = descending (m/s)
const float MAIN_DEPLOY_ALT     = 200.0f;  // Main chute altitude (meters AGL)
const float LANDING_VELOCITY    = 1.0f;    // Landing detection threshold (m/s)
const uint32_t LANDING_TIME_MS  = 5000;    // Must be slow for this long

// ============================================================================
// FLIGHT VARIABLES
// ============================================================================

static uint32_t landingStartTime = 0;
static bool landingDetectionStarted = false;

// ============================================================================
// USER SETUP - Called once at boot
// ============================================================================

void UserSetup() {
    // Initialize ARK system
    ark.begin();
    
    // Signal ready
#if ENABLE_BUZZER
    ark.buzzer.beep(2);
#endif
    
    ark.log("=== ARK Rocket Ready ===");
    ark.logf("Flight mode: %s", 
             ARK_FLIGHT_MODE == ARK_FLIGHT_MODE_SIMPLE ? "SIMPLE" : "ADVANCED");
}

// ============================================================================
// USER LOOP - Called repeatedly (~100 Hz)
// ============================================================================

void UserLoop() {
    // Update ARK (reads sensors, runs state machine)
    ark.update();
    
    // Heartbeat LED
#if ENABLE_LED
    ark.led.blink(500);
#endif
    
    // Handle each flight state
    switch (ark.state) {
        
        // ----- IDLE: Waiting for arm command -----
        case IDLE:
            // TODO: Check your arm switch or radio command here
            // Example: auto-arm after 5 seconds for testing
            // if (ark.uptime() > 5000) ark.arm();
            break;
        
        // ----- ARMED: Ready for launch -----
        case ARMED:
#if ENABLE_MPU9250
            // Detect launch by high acceleration
            if (ark.acceleration() > LAUNCH_ACCEL_G * 9.8f) {
                ark.log("LAUNCH DETECTED!");
                FlightManager.SetState(FLIGHT);
            }
#endif
            break;
        
        // ----- FLIGHT: Active flight -----
        case FLIGHT:
#if ENABLE_BMP280
            // Log telemetry
            static uint32_t lastLog = 0;
            if (ark.uptime() - lastLog > 200) {
                ark.logf("Alt: %.1fm  Vel: %.1fm/s  Max: %.1fm",
                         ark.altitude(), ark.velocity(), ark.maxAltitude());
                lastLog = ark.uptime();
            }
            
            // APOGEE DETECTION: Deploy drogue when velocity goes negative
            if (!ark.isDrogueDeployed() && ark.velocity() < APOGEE_VELOCITY) {
                ark.logf("APOGEE at %.1f m!", ark.maxAltitude());
                ark.deployDrogue();
                FlightManager.SetState(RECOVERY);
            }
#endif
            break;
        
        // ----- RECOVERY: Descending under chutes -----
        case RECOVERY:
#if ENABLE_BMP280
            // MAIN DEPLOYMENT: At altitude threshold
            if (!ark.isMainDeployed() && ark.isDrogueDeployed()) {
                if (ark.altitude() < MAIN_DEPLOY_ALT) {
                    ark.logf("MAIN DEPLOY at %.1f m", ark.altitude());
                    ark.deployMain();
                }
            }
            
            // LANDING DETECTION: Low velocity for sustained period
            if (ark.velocity() > -LANDING_VELOCITY && ark.velocity() < LANDING_VELOCITY) {
                if (!landingDetectionStarted) {
                    landingStartTime = ark.uptime();
                    landingDetectionStarted = true;
                } else if (ark.uptime() - landingStartTime > LANDING_TIME_MS) {
                    ark.log("LANDED!");
                    FlightManager.SetState(LANDED);
                }
            } else {
                landingDetectionStarted = false;
            }
#endif
            break;
        
        // ----- LANDED: Mission complete -----
        case LANDED:
            // Beep locator tone every 2 seconds
#if ENABLE_BUZZER
            static uint32_t lastBeep = 0;
            if (ark.uptime() - lastBeep > 2000) {
                ark.buzzer.tone(2000, 500);  // 2kHz for 500ms
                lastBeep = ark.uptime();
            }
#endif
            // Log final stats once
            static bool statsLogged = false;
            if (!statsLogged) {
                ark.log("========== FLIGHT COMPLETE ==========");
#if ENABLE_BMP280
                ark.logf("Max Altitude: %.1f meters", ark.maxAltitude());
#endif
                ark.logf("Flight Time:  %lu ms", ark.uptime());
                statsLogged = true;
            }
            break;
        
        // ----- FAULT: Error recovery -----
        case FAULT:
#if ENABLE_BUZZER
            ark.buzzer.beep(3);
#endif
            ark.log("FAULT - Check system!");
            // Attempt recovery
            FlightManager.SetState(IDLE);
            break;
    }
}
