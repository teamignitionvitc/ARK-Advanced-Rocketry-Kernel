#ifndef SEPARATION_STATE_HPP
#define SEPARATION_STATE_HPP

#include "../State_Util.hpp"

class SeparationState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::SEPARATION; }
};

#endif
