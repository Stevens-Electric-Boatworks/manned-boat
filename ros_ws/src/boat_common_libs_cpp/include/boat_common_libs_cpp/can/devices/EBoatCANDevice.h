//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <optional>

#include "boat_common_libs_cpp/can/devices/EBPDefaultProperties.h"
#include "boat_common_libs_cpp/can/CanDevice.h"

namespace eboat::can
{
    class EBoatCANDevice : public CanDevice
    {
    public:
        explicit EBoatCANDevice(const uint8_t can_id)
            : can_id(can_id)
        {
        }

        /**
         * This function MUST be overriden in children to handle custom CAN messages`
         */
        virtual void handle_custom_can_data(const sockcanpp::CanMessage& msg) = 0;
        void on_can_error(const sockcanpp::CanMessage& msg) override;
        void on_can_data(const sockcanpp::CanMessage& msg) override;

        [[nodiscard]] uint8_t get_can_id() override;

        [[nodiscard]] DeviceState get_device_state() const;

        [[nodiscard]] BoardType get_board_type() const;

        [[nodiscard]] const std::optional<PowerRails>& get_power_rails() const;

        [[nodiscard]] const std::optional<uint16_t>& get_seconds_since_boot() const;

        [[nodiscard]] const std::optional<uint16_t>& get_ms_since_heartbeat() const;

    protected:
        /**
         * Helper function o get the byte in the header
         * @param msg The CAN message
         * @return The byte in the header
         */
        static inline uint8_t get_header_byte(const sockcanpp::CanMessage& msg);
    private:
        void parse_generic_headers(const sockcanpp::CanMessage& msg);
        void parse_device_status_frame(const sockcanpp::CanMessage& msg);
        void parse_power_rails_frame(const sockcanpp::CanMessage& msg);


        // protocol default values
        DeviceState device_state_ = DeviceState::UNKNOWN;
        BoardType board_type_ = BoardType::UNKNOWN;
        std::optional<PowerRails> rails_ = std::nullopt;
        std::optional<uint16_t> seconds_since_boot = std::nullopt;
        std::optional<uint16_t> ms_since_heartbeat = std::nullopt;
        uint8_t can_id;
    };
}
