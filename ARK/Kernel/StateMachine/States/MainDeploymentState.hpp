#ifndef MAIN_DEPLOYMENT_STATE_HPP
#define MAIN_DEPLOYMENT_STATE_HPP

#include "../State_Util.hpp"

class MainDeploymentState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override { return FlightState::MAIN_DEPLOYMENT; }
};

#endif
