#ifndef CRUISING_STATE_HPP
#define CRUISING_STATE_HPP

#include "../State_Util.hpp"

class CruisingState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::CRUISING; }
};

#endif
