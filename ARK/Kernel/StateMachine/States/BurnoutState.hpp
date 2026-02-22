#ifndef BURNOUT_STATE_HPP
#define BURNOUT_STATE_HPP

#include "../State_Util.hpp"

class BurnoutState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::BURNOUT; }
};

#endif
