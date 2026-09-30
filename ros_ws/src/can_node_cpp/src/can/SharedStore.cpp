//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "../../include/can_node_cpp/SharedStore.h"

#include "can_node_cpp/can/Motors.h"

std::optional<eboat::CANData> eboat::SharedStore::getSDO(const Motors motor, const MotorSDOParam param) const
{
  this->get_queued_reads().push(param);
  if (const auto it = this->cached->find(motor); it != this->cached->end()) {
    if (const auto it2 = it->second.find(param); it2 != it->second. end()) {
      return it2->second;
    }
  }
  return std::nullopt;
}
void eboat::SharedStore::storeSDO(const Motors motor, const MotorSDOParam param, CANData data) const {
  if (const auto it = cached->find(motor); it != cached->end()) {
    it->second.insert_or_assign(param, data);
  } else {
    std::unordered_map<MotorSDOParam, CANData> map = {{param, data}};
    cached->insert_or_assign(motor, map);
  }

  if (this->onCANDataReceive != nullptr)
  {
    onCANDataReceive(motor, param, data);
  }
}
