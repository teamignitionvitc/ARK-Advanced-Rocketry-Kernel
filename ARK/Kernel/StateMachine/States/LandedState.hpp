#ifndef LANDED_STATE_HPP
#define LANDED_STATE_HPP

#include "../State_Util.hpp"

class LandedState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::LANDED; }
};

#endif
