//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/can/CANBusService.h"

#include <bits/signum-generic.h>
#include <lely/coapp/fiber_driver.hpp>
#include <lely/coapp/master.hpp>
#include <lely/ev/loop.hpp>
#include <lely/io2/linux/can.hpp>
#include <lely/io2/posix/poll.hpp>
#include <lely/io2/sys/sigset.hpp>
#include <lely/io2/sys/timer.hpp>

using namespace std::chrono_literals;

void eboat::CANBusService::initBus() {
  // Create an I/O context to synchronize I/O services during shutdown.
  lely::io::Context ctx;
  // Create a platform-specific I/O polling instance to monitor the CAN bus, as
  // well as timers and signals.
  const lely::io::Poll poll(ctx);
  // Create a polling event loop and pass it the platform-independent polling
  // interface. If no tasks are pending, the event loop will poll for I/O
  // events.
  lely::ev::Loop loop(poll.get_poll());
  // I/O devices only need access to the executor interface of the event loop.
  const auto exec = loop.get_executor();
  // Create a timer using a monotonic clock, i.e., a clock that is not affected
  // by discontinuous jumps in the system time.
  lely::io::Timer timer(poll, exec, CLOCK_MONOTONIC);
  const lely::io::CanController ctrl("vcan0");
  lely::io::CanChannel chan(poll, exec);
  chan.open(ctrl);

  // Create a CANopen master with node-ID 1. The master is asynchronous, which
  // means every user-defined callback for a CANopen event will be posted as a
  // task on the event loop, instead of being invoked during the event
  // processing by the stack.
  lely::canopen::AsyncMaster master(timer, chan, "/home/isayal/motors.eds", "", 1);

  // Create a driver for the slave with node-ID 2.
  CANDriver driver(exec, master, 6);

  // Create a signal handler.
  lely::io::SignalSet sigset(poll, exec);
  // Watch for Ctrl+C or process termination.
  sigset.insert(SIGHUP);
  sigset.insert(SIGINT);
  sigset.insert(SIGTERM);



  // Submit a task to be executed when a signal is raised. We don't care which.
  sigset.submit_wait([&](int /*signo*/) {
    // If the signal is raised again, terminate immediately.
    sigset.clear();
    // Tell the master to start the deconfiguration process for all nodes, and
    // submit a task to be executed once that process completes.
    master.AsyncDeconfig().submit(exec, [&]() {
      // Perform a clean shutdown.
      ctx.shutdown();
    });
  });

  // Start the NMT service of the master by pretending to receive a 'reset
  // node' command.
  master.Reset();
  // Run the event loop until no tasks remain (or the I/O context is shut down).
  loop.run();
}
bool eboat::CANBusService::initialized() {
  return false;
}
