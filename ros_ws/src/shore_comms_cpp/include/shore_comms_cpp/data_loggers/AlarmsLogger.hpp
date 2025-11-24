#pragma once

#include "builtin_interfaces/msg/time.hpp"
#include "shore_comms_cpp/IDataLogger.hpp"
#include "shore_comms_cpp/IDataReceiver.hpp"
#include <memory>
#include <boat_data_interfaces/msg/boat_alarm.hpp>
#include <boat_data_interfaces/msg/detail/can_motor_data__builder.hpp>
#include <rclcpp/rclcpp.hpp>

class AlarmsLogger : public IDataLogger<boat_data_interfaces::msg::BoatAlarm> {
public:
    using type = boat_data_interfaces::msg::BoatAlarm;
    explicit AlarmsLogger(const std::shared_ptr<IDataReceiver<type>>&, const std::shared_ptr<IDataTransmitter>&, bool);
    void on_data(type data) const;
};
