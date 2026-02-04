#ifndef FAILSAFE_STATE_HPP
#define FAILSAFE_STATE_HPP

#include "../State_Util.hpp"

class FailsafeState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::FAILSAFE; }
};

#endif
