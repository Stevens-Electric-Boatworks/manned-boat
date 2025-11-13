#pragma once

#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <boat_data_interfaces/msg/gps_data.hpp>
#include <rclcpp/rclcpp.hpp>


class GPSLogger : public IDataLogger<boat_data_interfaces::msg::GPSData> {
public:
    using type = boat_data_interfaces::msg::GPSData;
    explicit GPSLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(const type data) const;
};
