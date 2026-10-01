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
enum class ODType { U8, U16, U32, I8, I16, I32, PDO_UNKNOWN };

struct MotorODParam {
  uint16_t index;
  uint8_t subindex;
  ODType type;
  bool operator==(const MotorODParam&   other) const {
    return index == other.index && subindex == other.subindex;
  }
  friend std::size_t hash_value(const MotorODParam &obj) {
    std::size_t seed = 0x216DD4C1;
    seed ^= (seed << 6) + (seed >> 2) + 0x360C5B1B +
            static_cast<std::size_t>(obj.index);
    seed ^= (seed << 6) + (seed >> 2) + 0x50EC5801 +
            static_cast<std::size_t>(obj.subindex);
    return seed;
  }
};

inline constexpr MotorODParam k2030sub2    {.index = 0x2030, .subindex = 2, .type = ODType::I16};
inline constexpr MotorODParam k2030sub3 {.index = 0x2030, .subindex = 3, .type = ODType::I16};

inline constexpr std::array kKnownParams = {k2030sub2, k2030sub3};

inline const MotorODParam* findParam(uint16_t idx, uint8_t sub) {
  for (const auto& p : kKnownParams)
    if (p.index == idx && p.subindex == sub) return &p;
  return nullptr;
}

template <class T>
struct TypeTag { using type = T; };

  template <class F>
  void dispatchType(ODType t, F&& f) {
    switch (t) {
      case ODType::U8:  f(TypeTag<uint8_t>{});  break;
      case ODType::U16: f(TypeTag<uint16_t>{}); break;
      case ODType::U32: f(TypeTag<uint32_t>{}); break;
      case ODType::I8:  f(TypeTag<int8_t>{});   break;
      case ODType::I16: f(TypeTag<int16_t>{});  break;
      case ODType::I32: f(TypeTag<int32_t>{});  break;
      default: break;
    }
  }

}

namespace std {
template <>
struct hash<eboat::MotorODParam> {
  std::size_t operator()(const eboat::MotorODParam& obj) const noexcept {
    return hash_value(obj);
  }
};
}
