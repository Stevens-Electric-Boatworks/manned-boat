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

namespace eboat {

class CANBusService {
private:
  std::shared_ptr<lely::ev::Loop> _loop;
  std::shared_ptr<lely::io::Poll> _poll;
  std::shared_ptr<lely::io::Timer> _timer;
  std::shared_ptr<lely::io::CanController> _ctrl;
  std::shared_ptr<lely::io::CanChannel> _chan;

  bool _initialized = false;

  public:
  std::unique_ptr<CANMotor> motorA;
  std::unique_ptr<CANMotor> motorB;
  std::optional<lely::canopen::AsyncMaster> masterNode;
  void initBus();

  bool initialized() const;

  void periodic() const;

};
}