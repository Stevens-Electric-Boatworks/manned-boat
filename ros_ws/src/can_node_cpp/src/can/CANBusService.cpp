//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/can/CANBusService.h"
#include "can_node_cpp/can/CANMotor.h"

#include <csignal>
#include <iostream>
#include <lely/coapp/fiber_driver.hpp>
#include <lely/coapp/master.hpp>
#include <lely/ev/loop.hpp>
#include <lely/io2/linux/can.hpp>
#include <lely/io2/posix/poll.hpp>
#include <lely/io2/sys/sigset.hpp>
#include <lely/io2/sys/timer.hpp>

using namespace std::chrono_literals;


void eboat::CANBusService::initBus() {

  if (_initialized) {
    return;
  }

  std::cout << "Running initBus()" << "\n";

  // Create an I/O context to synchronize I/O services during shutdown.
  lely::io::Context ctx;
  // Create a platform-specific I/O polling instance to monitor the CAN bus, as
  // well as timers and signals.
  this->_poll = std::make_shared<lely::io::Poll>(ctx);
  // Create a polling event loop and pass it the platform-independent polling
  // interface. If no tasks are pending, the event loop will poll for I/O
  // events.
  _loop = std::make_shared<lely::ev::Loop>(_poll->get_poll());
  // I/O devices only need access to the executor interface of the event loop.
  const auto exec = _loop->get_executor();
  // Create a timer using a monotonic clock, i.e., a clock that is not affected
  // by discontinuous jumps in the system time.
  _timer = std::make_shared<lely::io::Timer>(*_poll, exec, CLOCK_MONOTONIC);
  _ctrl = std::make_shared<lely::io::CanController>("vcan0");
  _chan = std::make_shared<lely::io::CanChannel>(*_poll, exec);
  _chan->open(*_ctrl);

  // Create a CANopen master with node-ID 1. The master is asynchronous, which
  // means every user-defined callback for a CANopen event will be posted as a
  // task on the event loop, instead of being invoked during the event
  // processing by the stack.
  this->masterNode.emplace(*_timer, *_chan, "/home/isayal/motors.eds", "", 1);

  // Create a driver for the slave with node-ID 2.
  this->motorA = std::make_unique<CANMotor>(
    std::make_shared<CANDriver>(exec, this->masterNode.value(), 6),
      "Motor A",
      static_cast<int8_t>(6),
      std::pmr::vector<MotorSDOParam>{}
  );

  this->motorB = std::make_unique<CANMotor>(
  std::make_shared<CANDriver>(exec, this->masterNode.value(), 7),
    "Motor B",
    static_cast<int8_t>(7),
    std::pmr::vector<MotorSDOParam>{}
);

  this->masterNode.value().Reset();
  std::cout << this->masterNode->GetTimeout().count() << " timeout";
  _initialized = true;
}
bool eboat::CANBusService::initialized() const {
  return _initialized;
}
void eboat::CANBusService::periodic() const {
  _loop->run_one();
}
