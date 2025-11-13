#pragma once

#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <boat_data_interfaces/msg/gpsvtg_data.hpp>
#include <rclcpp/rclcpp.hpp>


class GPSVTGLogger : public IDataLogger<boat_data_interfaces::msg::GPSVTGData> {
public:
    using type = boat_data_interfaces::msg::GPSVTGData;
    explicit GPSVTGLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(const type data) const;
};
