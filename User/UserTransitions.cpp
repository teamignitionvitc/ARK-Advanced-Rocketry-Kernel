
#include "ARK/Ark.hpp"

void UserEvaluateTransitions(StatesManager& flightManager) {

    if (ARK::IsState(FlightState::BOOT)) {
        // ...
        // if (v> 100) {
        //     flightManager.SetState(FlightState::INITIALIZING);
        //}
    }
    
    if (ARK::IsState(FlightState::INITIALIZING)) {
        // ...
    }

    if (ARK::IsState(FlightState::CALIBRATION)) {
        // ...
    }

    if (ARK::IsState(FlightState::IDLE)) {
        // ...
    }

    if (ARK::IsState(FlightState::PRE_ARM)) {
        // ...
    }

    if (ARK::IsState(FlightState::ARMED)) {
        // ...
    }

    if (ARK::IsState(FlightState::LAUNCH)) {
        // ...
    }

    if (ARK::IsState(FlightState::ASCENT)) {
        // ...
    }

    if (ARK::IsState(FlightState::BURNOUT)) {
        // ...
    }

    if (ARK::IsState(FlightState::SEPARATION)) {
        // ...
    }

    if (ARK::IsState(FlightState::CRUISING)) {
        // ...
    }

    if (ARK::IsState(FlightState::COASTING)) {
        // ...
    }

    if (ARK::IsState(FlightState::APOGEE)) {
        // ...
    }

    if (ARK::IsState(FlightState::DROGUE)) {
        // ...
    }

    if (ARK::IsState(FlightState::DOUBLE_DEPLOYMENT)) {
        // ...
    }

    if (ARK::IsState(FlightState::MAIN_DEPLOYMENT)) {
        // ...
    }

    if (ARK::IsState(FlightState::DESCENT)) {
        // ...
    }

    if (ARK::IsState(FlightState::LANDING_DETECTION)) {
        // ...
    }

    if (ARK::IsState(FlightState::LANDED)) {
        // ...
    }

    if (ARK::IsState(FlightState::FAILSAFE)) {
        // ...
    }
}

void UserOnStateEnter(int stateEnum) { }

void UserOnStateExit(int stateEnum) { }
