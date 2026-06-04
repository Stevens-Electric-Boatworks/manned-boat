//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "../IState.h"
#include "can_node_cpp/can/CANBusService.h"

namespace eboat {
/**
 * This represents the initialization of the CAN node.
 * Handles connecting to the CAN bus, and setting up the slave node.
 */
class InitializationState: public IState {
  CANBusService & busService;
public:
  explicit InitializationState(CANBusService& canBus) : busService(canBus){};
  ~InitializationState() override;
  [[nodiscard]] bool isValid() const override {
    return true;
  }
  void onSwitch() const override;

  // nothing periodic to do
  void periodic() const override;

  void cleanup() const override;
};
}