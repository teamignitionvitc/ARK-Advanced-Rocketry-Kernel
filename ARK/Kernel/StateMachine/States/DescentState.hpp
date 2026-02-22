#ifndef DESCENT_STATE_HPP
#define DESCENT_STATE_HPP

#include "../State_Util.hpp"

class DescentState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::DESCENT; }
};

#endif
