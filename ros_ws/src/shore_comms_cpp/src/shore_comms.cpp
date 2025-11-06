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
#include "boat_data_interfaces/msg/outlet_coolant_data.hpp"
#include "boat_data_interfaces/msg/gps_data.hpp"
#include "boat_data_interfaces/msg/gpsvtg_data.hpp"
#include "boat_data_interfaces/msg/can_motor_data.hpp"
#include "boat_data_interfaces/msg/can_bus_status.hpp"
#include "boat_data_interfaces/msg/boat_alarm.hpp"
#include "rcl_interfaces/msg/log.hpp"
#include "builtin_interfaces/msg/time.hpp"

struct LogData {
    double_t timestamp;
    std::string msg;
    std::string filename;
    std::string function;
    uint32_t line;
    uint8_t level;
};
struct Alarm {
    int16_t id;
    double_t timestamp;
};

// Needs sudo apt install nlohmann-json3-dev
class ShoreCommsNode : public rclcpp::Node {
public:
    ShoreCommsNode()
        : Node("shore_comms_cpp") {
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
        this->log_topic<boat_data_interfaces::msg::GPSVTGData>("/motion/vtg",
                                                    &ShoreCommsNode::gps_vtg_collector);
        this->log_topic<boat_data_interfaces::msg::CANMotorData>("/motors/can_motor_data",
                                                                 &ShoreCommsNode::motor_collector);
        this->log_topic<boat_data_interfaces::msg::CANBusStatus>("/motors/can_bus_state",
                                                                 &ShoreCommsNode::bus_state_collector);
        this->log_topic<boat_data_interfaces::msg::BoatAlarm>("/alarm/shore/publish",
                                                              &ShoreCommsNode::alarms_collector);
        this->log_topic<rcl_interfaces::msg::Log>("/rosout",
                                                  &ShoreCommsNode::logs_collector);
        this->log_topic<builtin_interfaces::msg::Time>("/boat_time",
                                          &ShoreCommsNode::boat_time_collector);
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

    void gps_vtg_collector(const boat_data_interfaces::msg::GPSVTGData::SharedPtr msg) {
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
        this->CANBusState = msg->bus_state;
    }
    void boat_time_collector(const builtin_interfaces::msg::Time::SharedPtr msg) {
        addData("boat_time", getTimeFromMsg(*msg));
    };

    void alarms_collector(const boat_data_interfaces::msg::BoatAlarm::SharedPtr msg) {
        addAlarm(Alarm{msg->error_code, getTimeFromMsg(msg->timestamp)});
    }

    void logs_collector(const rcl_interfaces::msg::Log::SharedPtr msg) {
        addLog(LogData{getTimeFromMsg(msg->stamp), msg->msg, msg->file,msg->function,msg->line, msg->level});
    }

    void send_websocket_data() {
        if (this->connectionOpened && !data.empty()) {
            this->sendData();
            this->sendAlarms();
            this->sendLogs();
            this->sendCANBusState();
        }
    }

private:
    std::vector<std::shared_ptr<rclcpp::TimerBase> > timers_;
    std::vector<std::shared_ptr<rclcpp::SubscriptionBase> > subscriptions_;

    //Data that we will send to the shore
    std::vector<LogData> logs_;
    std::vector<Alarm> alarms_;
    json data;
    uint8_t CANBusState = boat_data_interfaces::msg::CANBusStatus::OFFLINE;

    ix::WebSocket websocket;
    bool connectionOpened = false;

    void configureWebsocket() {
        const std::string url("wss://shore.stevenseboat.org/api");
        websocket.setUrl(url);
        RCLCPP_INFO(this->get_logger(), "Connecting to %s", url.c_str());
        websocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
            if (msg->type == ix::WebSocketMessageType::Open) {
                this->connectionOpened = true;
                RCLCPP_INFO(this->get_logger(), "WebSocket connection established.");
            } else if (msg->type == ix::WebSocketMessageType::Error) {
                RCLCPP_ERROR(this->get_logger(), "WebSocket error: %s", msg->errorInfo.reason.c_str());
            }
        });
        websocket.start();
    }

    void sendData() {
        json j;
        j["type"] = "data";
        j["payload"] = data;
        websocket.send(j.dump());
        data.clear();
    }

    void sendLogs() {
        if (this->logs_.size() == 0) return;
        std::vector<json> logs;
        json j = {
            {"type", "log"}
        };
        for (LogData data: logs_) {
            logs.push_back({
                {"timestamp", data.timestamp},
                {"msg", data.msg},
                {"file", data.filename},
                {"function", data.function},
                {"line", data.line},
                {"level", data.level},
            });
        }
        this->logs_.clear();
        j["payload"] = logs;
        websocket.send(j.dump());
    }

    void sendAlarms() {
        if (this->alarms_.size() == 0) return;
        std::vector<json> alarms;

        for (Alarm data: alarms_) {
            json j = {
                {"type", "alarm"},
                {"action", "set"}
            };
            json payload = {
                {"id", data.id},
                {"timestamp", data.timestamp},
                {"message", "Not supported yet..."},
                {"type", "error"}
            };
            j["payload"] = payload;
            websocket.send(j.dump());
        }
        this->alarms_.clear();
    }

    void sendCANBusState() {
        const json j = {
            {"type", "can_bus"},
            {"state", this->CANBusState}
        };
        this->websocket.send(j.dump());
    }


    void addData(const std::string &name, const json &value) {
        this->data[name] = value;
    }

    void addLog(const LogData &log) {
        this->logs_.push_back(log);
    }

    void addAlarm(const Alarm &alarm) {
        this->alarms_.push_back(alarm);
    }

    static double getTimeFromMsg(const builtin_interfaces::msg::Time& time) {
        return time.sec * 1000.0 + time.nanosec / 1.0e6;

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
