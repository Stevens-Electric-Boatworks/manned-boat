//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/ControlLoop.h"

#include "can_node_cpp/state_lib/states/InitializationState.h"

#include <iostream>
void eboat::ControlLoop::initialize() {
  if (this->canBus == nullptr) {
    this->canBus = std::make_shared<CANBusService>();
    this->canBus->initBus();
  }
  if (currentState == nullptr) {
    std::cout << "new current state" << "\n";
      currentState = std::make_unique<InitializationState>(*canBus);
  }
  else {

  }
}
#include <rclcpp/logging.hpp>
void eboat::ControlLoop::tickPeriodic() const {
  if (this->currentState == nullptr) {
        std::cout<< "The current state is null!" << "\n";
    return;
  }
  this->currentState->periodic();
}
void eboat::ControlLoop::tickBus() {
  if (this->canBus == nullptr) {
    return;
  }
  canBus->periodic();
}

void eboat::ControlLoop::initializeBus() {

}

