//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/ControlLoop.h"

#include "can_node_cpp/state_lib/states/InitializationState.h"
#include "can_node_cpp/state_lib/states/StandbyState.h"

#include <iostream>
void eboat::ControlLoop::initialize(rclcpp::Node* node) {
  if (this->canBus == nullptr) {
    this->canBus = std::make_shared<CANBusService>(node);
    this->canBus->initBus();
  }
  if (currentState == nullptr) {
      currentState = std::make_unique<InitializationState>(*canBus, [this](const States s) {
        switchTo(s);
      });
      currentState->onSwitch();
  }
}
#include <rclcpp/logging.hpp>
void eboat::ControlLoop::tickPeriodic() const {
  if (this->currentState == nullptr) {
      RCLCPP_ERROR(this->canBus->shared_store->logger, "Current state is null!");
    return;
  }
  this->currentState->periodic();
}
void eboat::ControlLoop::switchTo(const States state) {
  if (state == States::STANDBY) {
    currentState->cleanup();
    currentState = std::make_unique<StandbyState>(*canBus, [this](const States s) {
        switchTo(s);
      });
    currentState->onSwitch();
    RCLCPP_INFO(this->canBus->shared_store->logger, "Switched to STANDBY State");
  }
}

