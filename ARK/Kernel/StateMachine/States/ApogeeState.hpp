#ifndef APOGEE_STATE_HPP
#define APOGEE_STATE_HPP

#include "../State_Util.hpp"

class ApogeeState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::APOGEE; }
};

#endif
