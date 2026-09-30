//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "can/MotorSDOParam.h"

#include <any>
#include <chrono>
#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>

#include "PubManager.h"

namespace eboat {
struct CANData {
  std::any value;
  std::chrono::system_clock::time_point timestamp;
};
class SharedStore {
public:
  /**
   * All of the publishers available, including motor publishers
   */
  std::unique_ptr<PubManger> pubs;

  explicit SharedStore(rclcpp::Node* node)
  {
    pubs = std::make_unique<PubManger>(node);
  }

  [[nodiscard]] std::optional<CANData> getSDO(MotorSDOParam param);
  void storeSDO(MotorSDOParam param, CANData data) const;
  [[nodiscard]] std::queue<MotorSDOParam>& get_queued_reads() {
    return *queuedReads;
  }

private:
  std::shared_ptr<std::unordered_map<MotorSDOParam, CANData>> cached = std::make_shared<std::unordered_map<MotorSDOParam, CANData>>();
  std::shared_ptr<std::queue<MotorSDOParam>> queuedReads = std::make_shared<std::queue<MotorSDOParam>>();
};
}
