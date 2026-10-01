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
void eboat::StandbyState::onSwitch() const {}
void eboat::StandbyState::periodic() const {
  // std::cout << "Standby Periodic Called\n";
  auto value = busService.shared_store->getSDO(
    Motors::MOTOR_A,
    MotorODParam{
    .index = 0x2030,
    .subindex = 3,
    .type = ODType::I16
  });
}
void eboat::StandbyState::cleanup() const {}
