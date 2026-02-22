#ifndef DROGUE_STATE_HPP
#define DROGUE_STATE_HPP

#include "../State_Util.hpp"

class DrogueState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::DROGUE; }
};

#endif
