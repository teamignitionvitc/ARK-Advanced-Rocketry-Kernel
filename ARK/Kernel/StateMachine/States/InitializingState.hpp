#ifndef INITIALIZING_STATE_HPP
#define INITIALIZING_STATE_HPP

#include "../State_Util.hpp"

class InitializingState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::INITIALIZING; }
};

#endif
