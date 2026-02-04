#ifndef LANDING_DETECTION_STATE_HPP
#define LANDING_DETECTION_STATE_HPP

#include "../State_Util.hpp"

class LandingDetectionState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override {
    return FlightState::LANDING_DETECTION;
  }
};

#endif
