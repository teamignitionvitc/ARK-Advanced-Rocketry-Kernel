#ifndef DOUBLE_DEPLOYMENT_STATE_HPP
#define DOUBLE_DEPLOYMENT_STATE_HPP

#include "../State_Util.hpp"

class DoubleDeploymentState : public State {
public:
  void Enter() override;
  void Execute() override;
  void Exit() override;
  FlightState GetName() const override {
    return FlightState::DOUBLE_DEPLOYMENT;
  }
};

#endif
