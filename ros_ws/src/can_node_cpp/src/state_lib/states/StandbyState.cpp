//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/states/StandbyState.h"

#include "can_node_cpp/can/CANBusService.h"

#include <iostream>
eboat::StandbyState::~StandbyState() = default;
bool eboat::StandbyState::isValid() const {
  //TODO implement
  return false;
}
void eboat::StandbyState::onSwitch() const {
    std::cout << "Standby onSwitch() called!";
}
void eboat::StandbyState::periodic() const {
  // std::cout << "Standby Periodic Called\n";
  auto value = busService.shared_store->getSDO(MotorSDOParam{
    .index = 0x2030,
    .subindex = 2
  });
  if (value) {
    auto can_data = std::any_cast<int16_t>(value.value().value);
    std::cout << "CAN Data: " << can_data << "\n" << std::endl;
  }
  auto value2 = busService.shared_store->getSDO(MotorSDOParam{
    .index = 0x2030,
    .subindex = 3
  });
  if (value2) {
    auto can_data = std::any_cast<int16_t>(value2.value().value);
    std::cout << "CAN Data: " << can_data << "\n" << std::endl;
  }
}
void eboat::StandbyState::cleanup() const {}
