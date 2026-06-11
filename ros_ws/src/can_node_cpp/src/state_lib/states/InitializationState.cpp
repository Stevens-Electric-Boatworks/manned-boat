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
  // std::cout<< "Finished running bus service periodic" << "\n";

  auto x = this->busService.motorB->read({
  0x2030,
  2}
  );
}
void eboat::InitializationState::cleanup() const {}
