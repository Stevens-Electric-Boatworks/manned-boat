//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include "CANMotor.h"

#include <lely/coapp/fiber_driver.hpp>
#include <lely/ev/loop.hpp>
#include <lely/io2/linux/can.hpp>
#include <lely/io2/posix/poll.hpp>
#include <lely/io2/sys/timer.hpp>
#include <optional>
#include <thread>
#include <rclcpp/publisher.hpp>
#include <std_msgs/msg/int16.hpp>

namespace eboat {

class CANBusService {
private:
  std::shared_ptr<lely::io::Context> ctx;
  std::shared_ptr<lely::io::Poll> _poll;
  std::shared_ptr<lely::io::Timer> _timer;
  std::shared_ptr<lely::io::CanController> _ctrl;
  std::shared_ptr<lely::io::CanChannel> _chan;
  std::thread _ioThread;
  bool _initialized = false;

public:
  std::shared_ptr<lely::ev::Loop> loop;
  std::unique_ptr<CANMotor> motorA;
  std::unique_ptr<CANMotor> motorB;
  std::optional<lely::canopen::AsyncMaster> masterNode;
  std::unique_ptr<SharedStore> shared_store = std::make_unique<SharedStore>();
  rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr publisher;
  bool initBus(rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr);

  [[nodiscard]] bool initialized() const;

  void periodic() const;

};
}
