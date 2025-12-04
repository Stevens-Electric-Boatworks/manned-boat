#pragma once

#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <boat_data_interfaces/msg/shore_boat_alarm.hpp>
#include <rclcpp/rclcpp.hpp>

class AlarmsLogger : public IDataLogger<boat_data_interfaces::msg::ShoreBoatAlarm> {
public:
    using type = boat_data_interfaces::msg::ShoreBoatAlarm;
    explicit AlarmsLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(type data) const;
};
