
#include "ARK/Ark.hpp"

static float accelZ_bias = 0.0f;
static float accelMag_bias = 0.0f;
static float alt_bias = 0.0f;

static float last_altitude = 0.0f;
static float max_altitude  = 0.0f;

constexpr float LAUNCH_ACCEL_Z      = 30.0f;   // m/s^2
constexpr float BURNOUT_ACCEL_MAG   = 5.0f;    // m/s^2
constexpr float MAIN_DEPLOY_ALT     = 300.0f;  // meters
constexpr float LANDING_ACCEL_MAG   = 1.5f;    // m/s^2

static float moving_altitude_threshold = 5.0f + alt_bias; // meters

static bool drogue_fired = false;
static bool main_fired   = false;

void UserEvaluateTransitions(StatesManager& flightManager) {

    if (ARK::IsState(FlightState::BOOT)) {
        flightManager.SetState(FlightState::INITIALIZING);
    }

    if (ARK::IsState(FlightState::INITIALIZING)) {
        bool initialized = initAll();
        if (initialized){
            flightManager.SetState(FlightState::CALIBRATION);
        } else {
            // Handle initialization failure (e.g., retry, enter failsafe, etc.)
            flightManager.SetState(FlightState::BOOT);
        }
    }

    if (ARK::IsState(FlightState::CALIBRATION)) {

        bool isHealthy = checkAllSystems();
        if (isHealthy) {
            calibrateAllSensors();
            flightManager.SetState(FlightState::ARMED);
        } else {
            // Handle calibration failure (e.g., retry, enter failsafe, etc.)
            flightManager.SetState(FlightState::INITIALIZING);
        }
    }

    if (ARK::IsState(FlightState::IDLE)) {
        // we not use
    }

    if (ARK::IsState(FlightState::PRE_ARM)) {
        // we dont need this state for now
    }

    if (ARK::IsState(FlightState::ARMED)) {
        if (getAltitude() < moving_altitude_threshold && getAccelerationZ() > LAUNCH_ACCEL_Z)  {
            flightManager.SetState(FlightState::LAUNCH);
        } else{
            flightManager.SetState(FlightState::ARMED);
        }
    }

    if (ARK::IsState(FlightState::LAUNCH)) {
        moving_altitude_threshold = last_altitude - 5.0f;
        if (getAltitude() > last_altitude) {
            last_altitude = getAltitude();
            flightManager.SetState(FlightState::ASCENT);
        } else if (getAccelerationY() > Y_ACCEL_THRESHOLD || getAccelerationX() > X_ACCEL_THRESHOLD)
        {
            flightManager.SetState(FlightState::FAILSAFE);
        }
        else {
            flightManager.SetState(FlightState::LAUNCH);
        }
        
    }

    if (ARK::IsState(FlightState::ASCENT)) {
        if (getAccelerationZ() < 0 && getVelocity() > 0)
        {
            flightManager.SetState(FlightState::CRUISING);
        }else if (getVelocity() < 0)
        {
            flightManager.SetState(FlightState::FAILSAFE);
        } else {
            flightManager.SetState(FlightState::ASCENT);
        }
        
    }

    if (ARK::IsState(FlightState::BURNOUT)) {
        // ...
    }

    if (ARK::IsState(FlightState::SEPARATION)) {
        // ...
    }

    if (ARK::IsState(FlightState::CRUISING)) {
        if (getVelocity() < 0){
            flightManager.SetState(FlightState::APOGEE);
        } else {
            flightManager.SetState(FlightState::CRUISING);
        }
    }

    if (ARK::IsState(FlightState::COASTING)) {
        // ...
    }

    if (ARK::IsState(FlightState::APOGEE)) {
        fireDrogue();
        drogue_fired = true;
        flightManager.SetState(FlightState::DROGUE);
    }

    if (ARK::IsState(FlightState::DROGUE)) {
        if (getAltitude() < MAIN_DEPLOY_ALT && !main_fired) {
            fireMain();
            main_fired = true;
            flightManager.SetState(FlightState::MAIN_DEPLOYMENT);
        } else {
            flightManager.SetState(FlightState::DROGUE);
        }
    }

    if (ARK::IsState(FlightState::DOUBLE_DEPLOYMENT)) {
        // ...
    }

    if (ARK::IsState(FlightState::MAIN_DEPLOYMENT)) {
        flightManager.SetState(FlightState::DESCENT);
    }

    if (ARK::IsState(FlightState::DESCENT)) {
        if (getAccelerationZ() == 0) {
            flightManager.SetState(FlightState::LANDED);
        } else {
            flightManager.SetState(FlightState::DESCENT);
        }
    }

    if (ARK::IsState(FlightState::LANDING_DETECTION)) {
        // ...
    }

    if (ARK::IsState(FlightState::LANDED)) {
        beaconOn();

    }

    if (ARK::IsState(FlightState::FAILSAFE)) {
        fireDrogue();
        drogue_fired = true;
        fireMain();
        main_fired = true;
        flightManager.SetState(FlightState::DESCENT);
    }
}

void UserOnStateEnter(int stateEnum) { }

void UserOnStateExit(int stateEnum) { }
