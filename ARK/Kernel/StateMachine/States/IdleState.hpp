#ifndef IDLE_STATE_HPP
#define IDLE_STATE_HPP

#include "../State_Util.hpp"

class IdleState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::IDLE; }
};

#endif
