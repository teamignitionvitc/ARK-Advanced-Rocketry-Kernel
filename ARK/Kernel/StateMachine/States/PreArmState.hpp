#ifndef PRE_ARM_STATE_HPP
#define PRE_ARM_STATE_HPP

#include "../State_Util.hpp"

class PreArmState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::PRE_ARM; }
};

#endif
