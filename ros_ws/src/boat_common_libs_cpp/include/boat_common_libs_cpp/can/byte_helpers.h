//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include <cstdint>
#include <stdexcept>
#include <string_view>

namespace eboat::can {


    inline std::uint8_t readU8(const std::string_view d, const std::size_t i) {
        if (i >= d.size()) throw std::out_of_range("CAN frame too short");
        return static_cast<std::uint8_t>(d[i]);   // char may be signed, so cast first
    }

    inline std::uint16_t readU16(const std::string_view d, const std::size_t i) {
        return static_cast<std::uint16_t>(readU8(d, i) |
                                          (readU8(d, i + 1) << 8));
    }

    inline std::uint32_t readU32(const std::string_view d, const std::size_t i) {
        return  static_cast<std::uint32_t>(readU8(d, i))
             | (static_cast<std::uint32_t>(readU8(d, i + 1)) << 8)
             | (static_cast<std::uint32_t>(readU8(d, i + 2)) << 16)
             | (static_cast<std::uint32_t>(readU8(d, i + 3)) << 24);
    }

    inline std::int16_t readI16(const std::string_view d, const std::size_t i) {
        return static_cast<std::int16_t>(readU16(d, i));
    }

}