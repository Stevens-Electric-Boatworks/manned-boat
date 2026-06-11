//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <can_node_cpp/can/CANBusService.h>
#include "IState.h"

namespace eboat {
class StateManager {
  std::unique_ptr<IState> currentState = nullptr;
public:
  std::shared_ptr<CANBusService> canBus;

  StateManager() = default;

  void initialize();

  void initializeBus();

  /**
   * Runs the state machine, and must be called periodically
   */
  void tickPeriodic() const;
  void tickBus();
};
}