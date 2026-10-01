//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "can/MotorODParam.h"

#include <any>
#include <chrono>
#include <memory>
#include <optional>
#include <queue>
#include <unordered_map>
#include <utility>

#include "PubManager.h"
#include "can/Motors.h"

namespace eboat {
struct CANData {
  std::any value;
  std::chrono::system_clock::time_point timestamp;
};

template <typename T>
class UniqueQueue {
  private:
    std::queue<T> q;
    std::unordered_set<T> s;

  public:
    // Push an item only if it doesn't already exist
    bool push(const T& value) {
      if (s.find(value) != s.end()) {
        return false;
      }
      q.push(value);
      s.insert(value);
      return true;
    }

    // Pop the front element
    void pop() {
      if (q.empty()) return;
      s.erase(q.front());
      q.pop();
    }

    const T& front() const { return q.front(); }
    [[nodiscard]] bool empty() const { return q.empty(); }
    [[nodiscard]] size_t size() const { return q.size(); }

    bool contains(const T& value) const {
      return s.find(value) != s.end();
    }
  };
class SharedStore {
public:
  /**
   * All of the publishers available, including motor publishers
   */
  rclcpp::Logger logger;
  std::unique_ptr<PubManger> pubs;
  std::function<void(Motors motor,MotorODParam param, CANData data)> onCANDataReceive = nullptr;

  explicit SharedStore(rclcpp::Node* node, rclcpp::Logger  logger) : logger(std::move(logger))
  {
    pubs = std::make_unique<PubManger>(node);
  }

  [[nodiscard]] std::optional<CANData> getSDO(Motors motor, MotorODParam param) const;
  void store(Motors motor, MotorODParam param, CANData data) const;

  [[nodiscard]] UniqueQueue<MotorODParam>& get_queued_reads() const
  {
    return *queuedReads;
  }

private:
  std::shared_ptr<std::unordered_map<Motors, std::unordered_map<MotorODParam, CANData>>> data = std::make_shared<
    std::unordered_map<Motors, std::unordered_map<MotorODParam, CANData>>>();
  std::shared_ptr<UniqueQueue<MotorODParam>> queuedReads = std::make_shared<UniqueQueue<MotorODParam>>();
};
}
