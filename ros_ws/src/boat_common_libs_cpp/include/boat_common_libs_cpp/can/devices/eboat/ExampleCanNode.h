//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "boat_common_libs_cpp/can/devices/EBoatCANDevice.h"

// THIS IS AN EXAMPLE FILE (for now)
namespace eboat::can::device
{
    /**
     * This is an EXAMPLE of how one can implement a CAN node
     */
    class ExampleCanNode : public EBoatCANDevice
    {
    public:
        explicit ExampleCanNode(const uint8_t can_id)
            : EBoatCANDevice(can_id)
        {
        }

        /**
         * you MUST override this function from EboatCanDevice!!
         */
        void handle_custom_can_data(const sockcanpp::CanMessage& msg) override;
        /**
         * @return The temp of the rear cooling loop A sensor
         */
        [[nodiscard]] const std::optional<double>& get_temp_a() const;

        /**
         * @return The temp of the rear cooling loop B sensor
         */
        [[nodiscard]] const std::optional<double>& get_temp_b() const;

        /**
         * @return The temp of the rear cooling loop C sensor
         */
        [[nodiscard]] const std::optional<double>& get_temp_c() const;

    private:
        std::optional<double> temp_a = std::nullopt;
        std::optional<double> temp_b = std::nullopt;
        std::optional<double> temp_c = std::nullopt;

        void parse_cooling_temp_senors_vals(const sockcanpp::CanMessage& msg);
    };
}
