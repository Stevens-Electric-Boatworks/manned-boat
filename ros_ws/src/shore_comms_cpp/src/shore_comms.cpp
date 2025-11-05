//
// Created by ishaan on 10/23/25.
//

#include <memory>

#include "ixwebsocket/IXWebSocket.h"
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"
#include <nlohmann/json.hpp>

// for convenience
using json = nlohmann::json;
#include "boat_data_interfaces/msg/inlet_coolant_data.hpp"
#include "boat_data_interfaces/msg/inlet_coolant_data.hpp"
#include "boat_data_interfaces/msg/outlet_coolant_data.hpp"
#include "boat_data_interfaces/msg/gps_data.hpp"
#include "boat_data_interfaces/msg/gps_speed.hpp"
#include "boat_data_interfaces/msg/can_motor_data.hpp"
#include "boat_data_interfaces/msg/can_bus_status.hpp"
#include "boat_data_interfaces/msg/boat_alarm.hpp"
#include "rcl_interfaces/msg/log.hpp"
#include "builtin_interfaces/msg/time.hpp"


// Needs sudo apt install nlohmann-json3-dev
class ShoreCommsNode : public rclcpp::Node {
public:
    ShoreCommsNode()
        : Node("shore_comms_cpp")
    {
        auto timer_callback = [this]() -> void {
            this->send_websocket_data();
        };
        auto timer_ = this->create_wall_timer(std::chrono::milliseconds(100), timer_callback);
        this->timers_.push_back(timer_);

        this->configureWebsocket();

        // Register collectors
        this->log_topic<boat_data_interfaces::msg::InletCoolantData>("/electrical/temp_sensors/in",
            &ShoreCommsNode::electrical_coolant_temp_collector_inlet);
        this->log_topic<boat_data_interfaces::msg::OutletCoolantData>("/electrical/temp_sensors/out",
            &ShoreCommsNode::electrical_coolant_temp_collector_outlet);
        this->log_topic<boat_data_interfaces::msg::GPSData>("/motion/gps",
            &ShoreCommsNode::gps_location_collector);
        this->log_topic<boat_data_interfaces::msg::GPSSpeed>("/motion/speed",
            &ShoreCommsNode::gps_speed_collector);
        this->log_topic<boat_data_interfaces::msg::CANMotorData>("/motors/can_motor_data",
            &ShoreCommsNode::motor_collector);
        this->log_topic<boat_data_interfaces::msg::CANBusStatus>("/motors/can_bus_state",
            &ShoreCommsNode::bus_state_collector);
        this->log_topic<boat_data_interfaces::msg::BoatAlarm>("/alarm/shore/publish",
            &ShoreCommsNode::alarms_collector);
        this->log_topic<rcl_interfaces::msg::Log>("/rosout",
            &ShoreCommsNode::logs_collector);
    }

    void electrical_coolant_temp_collector_inlet(const boat_data_interfaces::msg::InletCoolantData::SharedPtr msg) {
        addData("inlet_temp", msg->inlet_temp);
    }

    void electrical_coolant_temp_collector_outlet(const boat_data_interfaces::msg::OutletCoolantData::SharedPtr msg) {
        addData("outlet_temp", msg->outlet_temp);
    }

    void gps_location_collector(const boat_data_interfaces::msg::GPSData::SharedPtr msg) {
        addData("lat", msg->lat);
        addData("long", msg->lon);
    }

    void gps_speed_collector(const boat_data_interfaces::msg::GPSSpeed::SharedPtr msg) {
        addData("speed", msg->speed);
    }

    void motor_collector(const boat_data_interfaces::msg::CANMotorData::SharedPtr msg) {
        addData("voltage", msg->voltage);
        addData("throttle_mv", msg->throttle_mv);
        addData("throttle_percentage", msg->throttle_percentage);
        addData("rpm", msg->rpm);
        addData("torque", msg->torque);
        addData("motor_temp", msg->motor_temp);
        addData("current", msg->current);
        addData("power", msg->power);
    }

    void bus_state_collector(const boat_data_interfaces::msg::CANBusStatus::SharedPtr msg) {
        addData("can_bus_state", msg->bus_state);
    }

    void alarms_collector(const boat_data_interfaces::msg::BoatAlarm::SharedPtr msg) {
        json alarm_json = {
            {"error_code", msg->error_code},
            {"timestamp", (msg->timestamp.sec * 1000) + (msg->timestamp.nanosec / 1e6)}
        };
        addData("alarm", alarm_json);
    }

    void logs_collector(const rcl_interfaces::msg::Log::SharedPtr msg) {
        json log_entry = {
            {"timestamp", (msg->stamp.sec * 1000) + (msg->stamp.nanosec / 1e6)},
            {"msg", msg->msg},
            {"file", msg->file},
            {"function", msg->function},
            {"line", msg->line},
            {"level", msg->level},
            {"name", msg->name}
        };
        addData("log", log_entry);
    }

private:
    std::vector<std::shared_ptr<rclcpp::TimerBase>> timers_;
    std::vector<std::shared_ptr<rclcpp::SubscriptionBase>> subscriptions_;
    json data;
    ix::WebSocket websocket;
    bool connectionOpened = false;

    void configureWebsocket() {
        const std::string url("wss://shore.stevenseboat.org/api");
        websocket.setUrl(url);
        RCLCPP_INFO(this->get_logger(), "Connecting to %s", url.c_str());
        websocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
            if (msg->type == ix::WebSocketMessageType::Open) {
                this->connectionOpened = true;
                RCLCPP_INFO(this->get_logger(), "WebSocket connection established ✅");
            } else if (msg->type == ix::WebSocketMessageType::Error) {
                RCLCPP_ERROR(this->get_logger(), "WebSocket error: %s", msg->errorInfo.reason.c_str());
            }
        });
        websocket.start();
    }

    void send_websocket_data() {
        if (this->connectionOpened && !data.empty()) {
            json j;
            j["type"] = "data";
            j["payload"] = data;
            websocket.send(j.dump());
            data.clear();
        }
    }

    void addData(const std::string &name, const json &value) {
        this->data[name] = value;
    }

    template<typename T, typename M>
    void log_topic(const std::string &topic_name, M callback) {
        auto bound = std::bind(callback, this, std::placeholders::_1);
        auto sub = this->create_subscription<T>(topic_name, 10, bound);
        subscriptions_.push_back(sub);
        RCLCPP_INFO(this->get_logger(), "Listening on '%s'", topic_name.c_str());
    }
};


int main(int argc, char *argv[]) {
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<ShoreCommsNode>());
    rclcpp::shutdown();
    return 0;
}
