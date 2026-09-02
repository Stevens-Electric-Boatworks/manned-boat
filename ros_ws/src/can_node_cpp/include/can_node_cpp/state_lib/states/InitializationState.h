//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "../IState.h"
#include "States.h"
#include "can_node_cpp/can/CANBusService.h"

namespace eboat {
/**
 * This represents the initialization of the CAN node.
 * Handles connecting to the CAN bus, and setting up the slave node.
 */
class InitializationState: public IState {
  CANBusService & busService;
  std::function<void(States)> switchTo;
public:
  explicit InitializationState(CANBusService& canBus, const std::function<void(States)>& switchTo)
      : busService(canBus), switchTo(switchTo) {};  ~InitializationState() override;
  [[nodiscard]] bool isValid() const override {
    return true;
  }
  void onSwitch() const override;

  // nothing periodic to do
  void periodic() const override;

  void cleanup() const override;
};
}