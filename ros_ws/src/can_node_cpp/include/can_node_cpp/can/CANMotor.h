//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <optional>
#include <string>
#include <variant>
#include <vector>
/**
 * Defines a CAN SDO Read that can be used on the motor
 */
struct MotorSDOParam {
  std::byte index;
  int8_t subindex;
};

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
  typedef std::optional<std::variant<int, double, std::string>> MotorValue;
public:
  std::string name;
  int8_t can_id;
  /**
   * The valid list of SDO params that this motor will listen to and can read
   * from
   */
  std::pmr::vector<MotorSDOParam> params;

  /**
   * Reads from the CAN bus
   * @param sdo_param The SDO parameter to read from the CAN bus
   * @return The SDO value read, if it exists.
   */
  MotorValue read(MotorSDOParam sdo_param);

  /**
   *
   * @return Reads the list of motor faults from the Motor Controller
   */
  std::vector<MotorFault> readMotorFaults();
};
