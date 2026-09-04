//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "MotorSDOParam.h"

#include <any>
#include <chrono>
#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>

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
  std::shared_ptr<std::unordered_map<MotorSDOParam, CANData>> cached =     std::make_shared<std::unordered_map<MotorSDOParam, CANData>>();
  std::shared_ptr<std::queue<MotorSDOParam>> queuedReads = std::make_shared<std::queue<MotorSDOParam>>();
};
}