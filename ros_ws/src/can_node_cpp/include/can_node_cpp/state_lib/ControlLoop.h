//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "IState.h"
#include "states/States.h"
#include <can_node_cpp/can/CANBusService.h>

namespace eboat {
class ControlLoop {
  std::unique_ptr<IState> currentState = nullptr;
public:
  std::shared_ptr<CANBusService> canBus;

  ControlLoop() = default;

  void initialize();

  void initializeBus();

  /**
   * Runs the state machine, and must be called periodically
   */
  void tickPeriodic() const;
  void tickBus();

  void switchTo(States state);
};
}