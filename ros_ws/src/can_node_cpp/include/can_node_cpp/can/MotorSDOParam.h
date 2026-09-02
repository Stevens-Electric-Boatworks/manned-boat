//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

# pragma once
#include <cstdint>
#include <cstddef>
#include <functional>

namespace eboat {
/**
 * Defines a CAN SDO Read that can be used on the motor
 */
struct MotorSDOParam {
  uint16_t index;
  int8_t subindex;
  bool operator==(const MotorSDOParam&   other) const {
    return index == other.index && subindex == other.subindex;
  }
  friend std::size_t hash_value(const MotorSDOParam &obj) {
    std::size_t seed = 0x216DD4C1;
    seed ^= (seed << 6) + (seed >> 2) + 0x360C5B1B +
            static_cast<std::size_t>(obj.index);
    seed ^= (seed << 6) + (seed >> 2) + 0x50EC5801 +
            static_cast<std::size_t>(obj.subindex);
    return seed;
  }
};
}

namespace std {
template <>
struct hash<eboat::MotorSDOParam> {
  std::size_t operator()(const eboat::MotorSDOParam& obj) const noexcept {
    return hash_value(obj);
  }
};
}
