//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "IState.h"
#include "states/States.h"
#include <can_node_cpp/can/CANBusService.h>
#include <rclcpp/publisher.hpp>
#include <std_msgs/msg/int16.hpp>
#include <utility>

namespace eboat {
class ControlLoop {
  rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr publisher;
  std::unique_ptr<IState> currentState = nullptr;
public:
  std::shared_ptr<CANBusService> canBus;

  explicit ControlLoop(rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr  publisher)
    : publisher(std::move(publisher))
  {
  }

  void initialize();

  void initializeBus();

  /**
   * Runs the state machine, and must be called periodically
   */
  void tickPeriodic() const;
  void tickBus();

  void switchTo(States state);
};
}
