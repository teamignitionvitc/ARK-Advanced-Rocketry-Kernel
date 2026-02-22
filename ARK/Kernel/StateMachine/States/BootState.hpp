#ifndef BOOT_STATE_HPP
#define BOOT_STATE_HPP

#include "../State_Util.hpp"

class BootState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::BOOT; }
};

#endif
