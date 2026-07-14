//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "can_node_cpp/can/CANDriver.h"

#include <any>
#include <queue>
#include <unordered_map>
#include <optional>
#include <chrono>
#include <memory>

namespace eboat {
struct CANData {
  std::any value;
  std::chrono::system_clock::time_point timestamp;
};
class SharedStore {
public:
  SharedStore() = default;
  [[nodiscard]] std::optional<CANData> getSDO(MotorSDOParam param) const;
  void storeSDO(MotorSDOParam param, CANData data) const;
  [[nodiscard]] const std::queue<MotorSDOParam>& get_queued_reads() const {
    return *queuedReads;
  }

private:
  std::unique_ptr<std::unordered_map<MotorSDOParam, CANData>> cached{};
  std::unique_ptr<std::queue<MotorSDOParam>> queuedReads{};
};
}