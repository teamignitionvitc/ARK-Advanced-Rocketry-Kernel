#ifndef COASTING_STATE_HPP
#define COASTING_STATE_HPP

#include "../State_Util.hpp"

class CoastingState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::COASTING; }
};

#endif
