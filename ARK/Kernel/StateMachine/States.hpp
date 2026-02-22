#ifndef STATES_HPP
#define STATES_HPP

#include "../../../Config.hpp"
#include <cstdint>

/* =============================================================================
 * Flight State Enum (compact uint8_t - saves RAM)
 * 
 * SIMPLE mode uses 6 states: IDLE, ARMED, FLIGHT, RECOVERY, LANDED, FAULT
 * ADVANCED mode adds finer states for complex missions
 * =============================================================================
 */

#if ARK_FLIGHT_MODE == ARK_FLIGHT_MODE_SIMPLE
// Simple mode: 6 states for most rockets (~90% of use cases)
enum FlightState : uint8_t {
    IDLE = 0,       // System ready, waiting for arm command
    ARMED,          // Armed and ready for launch
    FLIGHT,         // Active flight (launch through apogee)
    RECOVERY,       // Descending with chutes deployed
    LANDED,         // On ground, mission complete
    FAULT           // Error state
};

// Total: 6 states
#define FLIGHT_STATE_COUNT 6

#else
// Advanced mode: 10 states for complex missions
enum FlightState : uint8_t {
    IDLE = 0,           // System ready, waiting for arm command
    ARMED,              // Armed and ready for launch
    LAUNCH,             // Motor ignition detected
    ASCENT,             // Powered flight phase
    BURNOUT,            // Motor burnout, unpowered ascent
    APOGEE,             // Peak altitude reached
    DROGUE_DEPLOY,      // Drogue chute deployment
    MAIN_DEPLOY,        // Main chute deployment
    LANDED,             // On ground, mission complete
    FAULT               // Error state
};

// Total: 10 states
#define FLIGHT_STATE_COUNT 10

#endif

/* =============================================================================
 * State Machine Class (switch-case dispatch, no virtual functions)
 * =============================================================================
 */
class StatesManager {
public:
    StatesManager();
    
    // Main tick function - call this from the kernel loop
    void Tick();
    
    // State control
    void SetState(FlightState newState);
    FlightState GetState() const;
    
    // User-friendly state query
    bool isArmed() const;
    bool isFlying() const;
    bool isLanded() const;
    bool hasFault() const;
    
    // State name for debugging
    const char* GetStateName() const;

private:
    FlightState currentState;
    uint32_t stateEntryTime;  // Timestamp when current state was entered
    
    // State handlers (non-virtual, called via switch-case)
    void handleIdle();
    void handleArmed();
    void handleFault();
    void handleLanded();
    
#if ARK_FLIGHT_MODE == ARK_FLIGHT_MODE_SIMPLE
    void handleFlight();
    void handleRecovery();
#else
    void handleLaunch();
    void handleAscent();
    void handleBurnout();
    void handleApogee();
    void handleDrogueDeploy();
    void handleMainDeploy();
#endif
};

// Global state manager instance
extern StatesManager FlightManager;

#endif