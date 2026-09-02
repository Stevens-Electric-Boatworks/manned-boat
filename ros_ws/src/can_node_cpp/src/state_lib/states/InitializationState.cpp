//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/states/InitializationState.h"

#include <iostream>
eboat::InitializationState::~InitializationState() =default;
void eboat::InitializationState::onSwitch() const {
  bool success = this->busService.initBus();
  if (success) {
    switchTo(States::STANDBY);
  }
}
void eboat::InitializationState::periodic() const {
  // ... nothing to do
}
void eboat::InitializationState::cleanup() const {}

