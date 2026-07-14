//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/can/SharedStore.h"

std::optional<eboat::CANData> eboat::SharedStore::getSDO(const MotorSDOParam param) const {
  if (const auto it = this->cached->find(param); it != this->cached->end()) {
    return it->second;
  }
  return std::nullopt;
}
void eboat::SharedStore::storeSDO(const MotorSDOParam param, CANData data) const {
  this->cached->insert_or_assign(param, data);
}
