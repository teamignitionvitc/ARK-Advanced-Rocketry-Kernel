#ifndef STATE_HPP
#define STATE_HPP

#include "States.hpp"

class StateManager;

class State {
public:
  virtual ~State() = default;

  virtual void Enter() = 0;
  virtual void Execute() = 0;
  virtual void Exit() = 0;
  virtual FlightState GetName() const = 0;

protected:
  State() = default;
};

#endif
