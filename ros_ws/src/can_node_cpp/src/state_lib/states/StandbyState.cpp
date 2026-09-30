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
    .subindex = 3,
    .type = SDOType::I16
  });

  if (value) {
    auto message = std_msgs::msg::Int16();
    message.data = std::any_cast<int16_t>(value.value().value);
    this->busService.shared_store->pubs->genericStatus->publish(message);
  }
  // auto val2 = busService.shared_store->getSDO(MotorSDOParam{
  //   .index = 0x2071,
  //   .subindex = 2,
  //   .type = SDOType::I16
  // });
  //
  // if (val2) {
  //   std::printf("%d\n", std::any_cast<int16_t>(val2.value().value));
  // }


}
void eboat::StandbyState::cleanup() const {}
