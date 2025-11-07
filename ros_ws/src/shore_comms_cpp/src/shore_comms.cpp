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
#include "boat_common_libs_cpp/AlarmPublisher.hpp"
#include "boat_data_interfaces/msg/boat_alarm.hpp"
#include "boat_data_interfaces/msg/can_bus_status.hpp"
#include "boat_data_interfaces/msg/can_motor_data.hpp"
#include "boat_data_interfaces/msg/gps_data.hpp"
#include "boat_data_interfaces/msg/gpsvtg_data.hpp"
#include "boat_data_interfaces/msg/inlet_coolant_data.hpp"
#include "boat_data_interfaces/msg/outlet_coolant_data.hpp"
#include "builtin_interfaces/msg/time.hpp"
#include "rcl_interfaces/msg/log.hpp"

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
  ShoreCommsNode() : Node("shore_comms_cpp") {
    this->alarmPub = std::make_shared<AlarmPublisher>(this);
    // replay mode config
    auto param_replay = rcl_interfaces::msg::ParameterDescriptor{};
    param_replay.description = "Is it in replay mode?";
    this->declare_parameter("replay_mode", false, param_replay);

    std::string logTopicName = "/rosout";
    if (this->get_parameter("replay_mode").as_bool()) {
      this->replay_mode = true;
      logTopicName = "/logout";
      double const timestamp = getTimeFromMsg(this->get_clock()->now());
      addLog(LogData{timestamp, "The shore server is in REPLAY MODE",
                     "REPLAY MODE", "REPLAY MODE", 67, 40});
      RCLCPP_INFO(this->get_logger(), "The shore server is in REPLAY mode");
    }
    // websocket config
    auto param_data_send = rcl_interfaces::msg::ParameterDescriptor{};
    param_data_send.description = "The data send rate in MS";
    this->declare_parameter("data_send", 100, param_data_send);
    auto timer_callback = [this]() -> void { this->send_websocket_data(); };
    auto const timer_ = this->create_wall_timer(
        std::chrono::milliseconds(this->get_parameter("data_send").as_int()),
        timer_callback);

    auto watchdog_callback = [this]() -> void { this->disconnect_watchdog(); };
    auto const _watchdog =
        this->create_wall_timer(std::chrono::seconds(1), watchdog_callback);

    this->timers_.push_back(timer_);
    this->timers_.push_back(_watchdog);
    this->configureWebsocket();

    // Register collectors
    this->log_topic<boat_data_interfaces::msg::InletCoolantData>(
        "/electrical/temp_sensors/in",
        &ShoreCommsNode::electrical_coolant_temp_collector_inlet);
    this->log_topic<boat_data_interfaces::msg::OutletCoolantData>(
        "/electrical/temp_sensors/out",
        &ShoreCommsNode::electrical_coolant_temp_collector_outlet);
    this->log_topic<boat_data_interfaces::msg::GPSData>(
        "/motion/gps", &ShoreCommsNode::gps_location_collector);
    this->log_topic<boat_data_interfaces::msg::GPSVTGData>(
        "/motion/vtg", &ShoreCommsNode::gps_vtg_collector);
    this->log_topic<boat_data_interfaces::msg::CANMotorData>(
        "/motors/can_motor_data", &ShoreCommsNode::motor_collector);
    this->log_topic<boat_data_interfaces::msg::CANBusStatus>(
        "/motors/can_bus_state", &ShoreCommsNode::bus_state_collector);
    this->log_topic<boat_data_interfaces::msg::BoatAlarm>(
        "/alarm/shore/publish", &ShoreCommsNode::alarms_collector);
    this->log_topic<rcl_interfaces::msg::Log>(logTopicName,
                                              &ShoreCommsNode::logs_collector);
    this->log_topic<builtin_interfaces::msg::Time>(
        "/boat_time", &ShoreCommsNode::boat_time_collector);
  }

  void electrical_coolant_temp_collector_inlet(
      const boat_data_interfaces::msg::InletCoolantData::SharedPtr msg) {
    addData("inlet_temp", msg->inlet_temp);
  }

  void electrical_coolant_temp_collector_outlet(
      const boat_data_interfaces::msg::OutletCoolantData::SharedPtr msg) {
    addData("outlet_temp", msg->outlet_temp);
  }

  void gps_location_collector(
      const boat_data_interfaces::msg::GPSData::SharedPtr msg) {
    addData("lat", msg->lat);
    addData("long", msg->lon);
  }

  void gps_vtg_collector(
      const boat_data_interfaces::msg::GPSVTGData::SharedPtr msg) {
    addData("speed", msg->speed);
  }

  void motor_collector(
      const boat_data_interfaces::msg::CANMotorData::SharedPtr msg) {
    addData("voltage", msg->voltage);
    addData("throttle_mv", msg->throttle_mv);
    addData("throttle_percentage", msg->throttle_percentage);
    addData("rpm", msg->rpm);
    addData("torque", msg->torque);
    addData("motor_temp", msg->motor_temp);
    addData("current", msg->current);
    addData("power", msg->power);
  }

  void bus_state_collector(
      const boat_data_interfaces::msg::CANBusStatus::SharedPtr msg) {
    this->CANBusState = msg->bus_state;
  }

  void boat_time_collector(const builtin_interfaces::msg::Time::SharedPtr msg) {
    addData("boat_time", getTimeFromMsg(*msg));
  };

  void
  alarms_collector(const boat_data_interfaces::msg::BoatAlarm::SharedPtr msg) {
    addAlarm(Alarm{msg->error_code, getTimeFromMsg(msg->timestamp)});
  }

  void logs_collector(const rcl_interfaces::msg::Log::SharedPtr msg) {
    addLog(LogData{getTimeFromMsg(msg->stamp), msg->msg, msg->file,
                   msg->function, msg->line, msg->level});
  }

  void send_websocket_data() {
    if (this->connectionOpened) {
      this->sendData();
      this->sendAlarms();
      this->sendLogs();
      this->sendCANBusState();
    }
  }

  void disconnect_watchdog() const {
    if (!this->connectionOpened && !this->openedInitally) {
      RCLCPP_ERROR(this->get_logger(),
                   "The websocket is not initally opened yet!");
      alarmPub->publishAlarm(Faults::WEBSOCKET_IS_NOT_INITIALLY_OPENED_YET);
    } else if (!this->websocket.getPingInterval()) {
      RCLCPP_ERROR(this->get_logger(),
                   "The websocket connection is now closed!");
      alarmPub->publishAlarm(Faults::WEBSOCKET_NOT_OPENED);
    }
  }

private:
  std::vector<std::shared_ptr<rclcpp::TimerBase>> timers_;
  std::vector<std::shared_ptr<rclcpp::SubscriptionBase>> subscriptions_;
  std::shared_ptr<AlarmPublisher> alarmPub;

  // Data that we will send to the shore
  std::vector<LogData> logs_;
  std::vector<Alarm> alarms_;
  json data;
  bool replay_mode = false;
  uint8_t CANBusState = boat_data_interfaces::msg::CANBusStatus::OFFLINE;

  ix::WebSocket websocket;
  bool connectionOpened = false;
  bool openedInitally = false;

  void configureWebsocket() {
    const std::string url("wss://shore.stevenseboat.org/api");
    websocket.setUrl(url);
    websocket.setMaxWaitBetweenReconnectionRetries(5);
    RCLCPP_INFO(this->get_logger(), "Connecting to %s", url.c_str());
    websocket.setOnMessageCallback([this](const ix::WebSocketMessagePtr &msg) {
      if (msg->type == ix::WebSocketMessageType::Open) {
        this->connectionOpened = true;
        this->openedInitally = true;
        RCLCPP_INFO(this->get_logger(), "WebSocket connection established.");

        json j = {{"type", "ident"}, {"message", "boat"}};
        websocket.send(j.dump());

        alarmPub->delatchAlarm(Faults::WEBSOCKET_CONNECTION_CLOSED);
        alarmPub->delatchAlarm(Faults::WEBSOCKET_INITIAL_CONNECTION_FAILURE);
        alarmPub->delatchAlarm(Faults::WEBSOCKET_NOT_OPENED);
        alarmPub->delatchAlarm(Faults::WEBSOCKET_IS_NOT_INITIALLY_OPENED_YET);
      } else if (msg->type == ix::WebSocketMessageType::Error) {
        this->connectionOpened = false;
        RCLCPP_ERROR(this->get_logger(), "WebSocket error: %s",
                     msg->errorInfo.reason.c_str());
        if (this->openedInitally) {
          alarmPub->publishAlarm(Faults::WEBSOCKET_INITIAL_CONNECTION_FAILURE);
        } else {
          alarmPub->publishAlarm(Faults::WEBSOCKET_CONNECTION_CLOSED);
        }
      } else {
        RCLCPP_INFO(this->get_logger(), "Something happened? %s",
                    msg->errorInfo.reason.c_str());
      }
    });
    websocket.start();
  }

  void sendData() {
    json j;
    j["type"] = "data";
    if (this->replay_mode) {
      data["replay"] = true;
    }
    j["payload"] = data;
    websocket.send(j.dump());
    data.clear();
  }

  void sendLogs() {
    if (this->logs_.size() == 0)
      return;
    std::vector<json> logs;
    json j = {{"type", "log"}};
    for (LogData data : logs_) {
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
    if (this->alarms_.size() == 0)
      return;
    std::vector<json> alarms;

    for (Alarm data : alarms_) {
      json j = {{"type", "alarm"}, {"action", "set"}};
      json payload = {{"id", data.id},
                      {"timestamp", data.timestamp},
                      {"message", "Not supported yet..."},
                      {"type", "error"}};
      j["payload"] = payload;
      websocket.send(j.dump());
    }
    this->alarms_.clear();
  }

  void sendCANBusState() {
    const json j = {{"type", "can_bus"}, {"state", this->CANBusState}};
    this->websocket.send(j.dump());
  }

  void addData(const std::string &name, const json &value) {
    this->data[name] = value;
  }

  void addLog(const LogData &log) { this->logs_.push_back(log); }

  void addAlarm(const Alarm &alarm) { this->alarms_.push_back(alarm); }

  static double getTimeFromMsg(const builtin_interfaces::msg::Time &time) {
    return time.sec * 1000.0 + time.nanosec / 1.0e6;
  }

  template <typename T, typename M>
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
