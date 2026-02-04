#ifndef ARMED_STATE_HPP
#define ARMED_STATE_HPP

#include "../State_Util.hpp"

class ArmedState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::ARMED; }
};

#endif
