#include "States.hpp"
#include "../../../Config.hpp"
#include "../../HAL/Time/Time.hpp"

// Global state manager instance
StatesManager FlightManager;

StatesManager::StatesManager() 
    : currentState(FlightState::IDLE)
    , stateEntryTime(0) 
{}

void StatesManager::SetState(FlightState newState) {
    if (currentState != newState) {
        currentState = newState;
        stateEntryTime = ARK::SystemTime.GetMillis();
    }
}

FlightState StatesManager::GetState() const {
    return currentState;
}

bool StatesManager::isArmed() const {
    return currentState == FlightState::ARMED;
}

bool StatesManager::isFlying() const {
#if ARK_FLIGHT_MODE == ARK_FLIGHT_MODE_SIMPLE
    return currentState == FlightState::FLIGHT || 
           currentState == FlightState::RECOVERY;
#else
    return currentState == FlightState::LAUNCH ||
           currentState == FlightState::ASCENT ||
           currentState == FlightState::BURNOUT ||
           currentState == FlightState::APOGEE ||
           currentState == FlightState::DROGUE_DEPLOY ||
           currentState == FlightState::MAIN_DEPLOY;
#endif
}

bool StatesManager::isLanded() const {
    return currentState == FlightState::LANDED;
}

bool StatesManager::hasFault() const {
    return currentState == FlightState::FAULT;
}

const char* StatesManager::GetStateName() const {
#if ARK_FLIGHT_MODE == ARK_FLIGHT_MODE_SIMPLE
    static const char* names[] = {
        "IDLE", "ARMED", "FLIGHT", "RECOVERY", "LANDED", "FAULT"
    };
#else
    static const char* names[] = {
        "IDLE", "ARMED", "LAUNCH", "ASCENT", "BURNOUT", 
        "APOGEE", "DROGUE_DEPLOY", "MAIN_DEPLOY", "LANDED", "FAULT"
    };
#endif
    uint8_t idx = static_cast<uint8_t>(currentState);
    if (idx < FLIGHT_STATE_COUNT) {
        return names[idx];
    }
    return "UNKNOWN";
}

/* =============================================================================
 * Main Tick Function - switch-case dispatch (NO virtual functions)
 * =============================================================================
 */
void StatesManager::Tick() {
#if ARK_FLIGHT_MODE == ARK_FLIGHT_MODE_SIMPLE
    // SIMPLE mode: 6 states
    switch (currentState) {
        case FlightState::IDLE:
            handleIdle();
            break;
        case FlightState::ARMED:
            handleArmed();
            break;
        case FlightState::FLIGHT:
            handleFlight();
            break;
        case FlightState::RECOVERY:
            handleRecovery();
            break;
        case FlightState::LANDED:
            handleLanded();
            break;
        case FlightState::FAULT:
            handleFault();
            break;
    }
#else
    // ADVANCED mode: 10 states
    switch (currentState) {
        case FlightState::IDLE:
            handleIdle();
            break;
        case FlightState::ARMED:
            handleArmed();
            break;
        case FlightState::LAUNCH:
            handleLaunch();
            break;
        case FlightState::ASCENT:
            handleAscent();
            break;
        case FlightState::BURNOUT:
            handleBurnout();
            break;
        case FlightState::APOGEE:
            handleApogee();
            break;
        case FlightState::DROGUE_DEPLOY:
            handleDrogueDeploy();
            break;
        case FlightState::MAIN_DEPLOY:
            handleMainDeploy();
            break;
        case FlightState::LANDED:
            handleLanded();
            break;
        case FlightState::FAULT:
            handleFault();
            break;
    }
#endif
}

/* =============================================================================
 * Common State Handlers
 * =============================================================================
 */
void StatesManager::handleIdle() {
    // Waiting for arm command
    // User can transition to ARMED via ark.arm() or external command
}

void StatesManager::handleArmed() {
    // System is armed, monitoring for launch detection
    // Typical trigger: acceleration > threshold
}

void StatesManager::handleLanded() {
    // Mission complete, beep locator tone, log final data
}

void StatesManager::handleFault() {
    // Error state - attempt recovery or safe mode
}

/* =============================================================================
 * SIMPLE Mode State Handlers
 * =============================================================================
 */
#if ARK_FLIGHT_MODE == ARK_FLIGHT_MODE_SIMPLE

void StatesManager::handleFlight() {
    // Active flight - from launch detection to recovery deployment
    // User code handles:
    // - Apogee detection (velocity < 0)
    // - Drogue deployment
    // - Main deployment (altitude check)
    // Transition to RECOVERY when main is deployed
}

void StatesManager::handleRecovery() {
    // Descending under chutes
    // Monitor for landing detection (velocity ~0 for sustained period)
}

#else

/* =============================================================================
 * ADVANCED Mode State Handlers
 * =============================================================================
 */
void StatesManager::handleLaunch() {
    // Motor ignition detected
    // Transition to ASCENT when liftoff confirmed
}

void StatesManager::handleAscent() {
    // Powered flight phase
    // Monitor for burnout (acceleration drops)
}

void StatesManager::handleBurnout() {
    // Unpowered ascent (coasting to apogee)
    // Monitor for apogee (velocity crosses zero)
}

void StatesManager::handleApogee() {
    // Peak altitude reached
    // Deploy drogue immediately
}

void StatesManager::handleDrogueDeploy() {
    // Drogue chute deployed, descending
    // Monitor altitude for main deployment
}

void StatesManager::handleMainDeploy() {
    // Main chute deployed, final descent
    // Monitor for landing
}

#endif