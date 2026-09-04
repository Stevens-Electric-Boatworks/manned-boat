//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/ControlLoop.h"

#include "can_node_cpp/state_lib/states/InitializationState.h"
#include "can_node_cpp/state_lib/states/StandbyState.h"

#include <iostream>
void eboat::ControlLoop::initialize() {
  if (this->canBus == nullptr) {
    this->canBus = std::make_shared<CANBusService>();
  }
  if (currentState == nullptr) {
    std::cout << "new current state" << "\n";
      currentState = std::make_unique<InitializationState>(*canBus, [this](const States s) {
        switchTo(s);
      });
      currentState->onSwitch();
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
void eboat::ControlLoop::switchTo(States state) {
  if (state == States::STANDBY) {
    currentState->cleanup();
    currentState = std::make_unique<StandbyState>(*canBus, [this](const States s) {
        switchTo(s);
      });
    currentState->onSwitch();
    std::cout << "Switched to Standby State!";
  }
}

void eboat::ControlLoop::initializeBus() {

}

