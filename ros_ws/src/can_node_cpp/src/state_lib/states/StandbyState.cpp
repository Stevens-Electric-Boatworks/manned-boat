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
    std::printf("%d\n", static_cast<uint16_t>(value.value().value));
  }
  auto val2 = busService.shared_store->getSDO(MotorSDOParam{
    .index = 0x2030,
    .subindex = 3
  });

  if (val2) {
    std::printf("%d\n", static_cast<uint16_t>(val2.value().value));
  }
  busService.shared_store->getSDO(MotorSDOParam{
    .index = 0x2030,
    .subindex = 5
  });
  busService.shared_store->getSDO(MotorSDOParam{
  .index = 0x2030,
  .subindex = 6
});
  busService.shared_store->getSDO(MotorSDOParam{
  .index = 0x2030,
  .subindex = 7
});
}
void eboat::StandbyState::cleanup() const {}
