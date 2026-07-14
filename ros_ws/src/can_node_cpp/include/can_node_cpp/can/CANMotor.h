//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include "CANDriver.h"

#include <memory>
#include <optional>
#include <string>
#include <utility>
#include <variant>
#include <vector>

namespace eboat {

//TODO: Implement proper motor fault detection
struct MotorFault {
  int event_id;
};
/**
 * Defines an Inmotion DCS CANOpen Motor Controller.
 */
class CANMotor {
  /**
   * Represents the different kinds of value that an SDO read can have
   */

  /**
   * The internal Lely CANOpen driver which handles this object
   */
public:
  std::string name;
  int8_t can_id;
  std::shared_ptr<CANDriver> canDriver;
  /**
   * The valid list of SDO params that this motor will listen to and can read
   * from
   */
  std::pmr::vector<MotorSDOParam> params;

  explicit CANMotor(std::shared_ptr<CANDriver> can_diver,
           std::string name,
           const int8_t can_id,
           const std::pmr::vector<MotorSDOParam> &params)
      : name(std::move(name)), can_id(can_id), canDriver(std::move(can_diver)), params(params) {}

  /**
   * Reads from the CAN bus
   * @param sdo_param The SDO parameter to read from the CAN bus
   * @return The SDO value read, if it exists.
   */
  [[nodiscard]] std::optional<std::any> read(SharedStore& shared_store, MotorSDOParam sdo_param) const;

  /**
   *
   * @return Reads the list of motor faults from the Motor Controller
   */
  std::vector<MotorFault> readMotorFaults();
};
}
