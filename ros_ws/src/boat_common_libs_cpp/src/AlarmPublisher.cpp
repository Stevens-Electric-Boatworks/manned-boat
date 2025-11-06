//
// Created by ishaan on 11/6/25.
//

#include "boat_common_libs_cpp/AlarmPublisher.hpp"
#include "boat_data_interfaces/srv/alarm_delatch.hpp"
#include "boat_data_interfaces/srv/alarm_raise.hpp"




AlarmPublisher::AlarmPublisher(rclcpp::Node* node) {
    RCLCPP_INFO(node->get_logger(), "Hello from the Alarm Library!!");
    this->node = node;
    this->alarmDeLatchClient = node->create_client<boat_data_interfaces::srv::AlarmDelatch>("/alarm/delatch");
    this->alarmRaiseClient = node->create_client<boat_data_interfaces::srv::AlarmRaise>("/alarm/raise");
}

void AlarmPublisher::publishAlarm(const Faults fault) const {
    const auto request = std::make_shared<boat_data_interfaces::srv::AlarmRaise::Request>();
    auto alarm = boat_data_interfaces::msg::BoatAlarm();
    alarm.error_code = static_cast<int8_t>(fault);
    alarm.timestamp = this->node->get_clock()->now();
    request->alarm = alarm;
    this->alarmRaiseClient->async_send_request(request);
}
