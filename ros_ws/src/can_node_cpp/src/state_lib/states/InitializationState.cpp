//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/states/InitializationState.h"

#include <iostream>
eboat::InitializationState::~InitializationState() =default;
void eboat::InitializationState::onSwitch() const {
  this->busService.initBus();
}
void eboat::InitializationState::periodic() const {
        // std::cout<< "Going to run bus service periodic" << "\n";
  // busService.periodic();

  auto x = this->busService.motorB->read({
  0x2030,
  2}
  );
  std::cout<< "Finished running init state periodic" << "\n";
}
void eboat::InitializationState::cleanup() const {}
