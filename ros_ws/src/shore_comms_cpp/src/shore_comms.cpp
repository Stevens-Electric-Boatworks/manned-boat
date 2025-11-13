  //
  // Created by ishaan on 10/23/25.
  //

  #include <memory>

#include "builtin_interfaces/msg/time.hpp"
#include "rcl_interfaces/msg/log.hpp"
#include "shore_comms_cpp/DataLoggers.hpp"
#include "shore_comms_cpp/data_loggers/BoatTimeLogger.hpp"
#include "shore_comms_cpp/data_loggers/CANBusStateLogger.hpp"
#include "shore_comms_cpp/data_loggers/MotorDataLogger.hpp"
#include "shore_comms_cpp/data_loggers/ROSOutLogger.hpp"
#include <nlohmann/json.hpp>

#include "shore_comms_cpp/data_loggers/AlarmsLogger.hpp"
#include "shore_comms_cpp/data_loggers/GPSLogger.hpp"
#include "shore_comms_cpp/data_loggers/GPSVTGLogger.hpp"
#include "shore_comms_cpp/data_loggers/InletCoolantLogger.hpp"
#include "shore_comms_cpp/data_loggers/OutletCoolantLogger.hpp"
  // for convenience
using json = nlohmann::json;
#include "boat_data_interfaces/msg/can_motor_data.hpp"



// Needs sudo apt install nlohmann-json3-dev
class ShoreCommsNode : public rclcpp::Node {
public:
  ShoreCommsNode() : Node("shore_comms_cpp") {}

  void init() {
    // replay mode config
    auto param_replay = rcl_interfaces::msg::ParameterDescriptor{};
    param_replay.description = "Is it in replay mode?";
    this->declare_parameter("replay_mode", false, param_replay);
    // websocket config
    auto param_data_send = rcl_interfaces::msg::ParameterDescriptor{};
    param_data_send.description = "The data send rate in MS";
    this->declare_parameter("data_send", 100, param_data_send);
    this->data_loggers = std::make_shared<DataLoggers>();
    this->data_loggers->set_replay_mode(this->get_parameter("replay_mode").as_bool());
    this->data_loggers->use_websockets(this->shared_from_this(), this->get_parameter("data_send").as_int());
    this->log_data<rcl_interfaces::msg::Log, ROSOutLogger>("/rosout");
    this->log_data<boat_data_interfaces::msg::CANBusStatus, CANBusStateLogger>("/motors/can_bus_state");
    this->log_data<boat_data_interfaces::msg::CANMotorData, MotorDataLogger>("/motors/can_motor_data");
    this->log_data<builtin_interfaces::msg::Time, BoatTimeLogger>("/boat_time");
    this->log_data<boat_data_interfaces::msg::BoatAlarm, AlarmsLogger>("/alarm/shore/publish");
    this->log_data<boat_data_interfaces::msg::InletCoolantData, InletCoolantLogger>("/electrical/temp_sensors/in");
    this->log_data<boat_data_interfaces::msg::OutletCoolantData, OutletCoolantLogger>("/electrical/temp_sensors/out");
    this->log_data<boat_data_interfaces::msg::GPSData, GPSLogger>("/motion/gps");
    this->log_data<boat_data_interfaces::msg::GPSVTGData, GPSVTGLogger>("/motion/vtg");


    RCLCPP_INFO(this->get_logger(), "Shore Node Initization Complete");
  }

private:
  std::shared_ptr<DataLoggers> data_loggers;

  template<typename T, typename L>
  void log_data(const std::string& topic_name) {
    this->data_loggers->addDataLogger<T, L>(topic_name, this->shared_from_this());
  }
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  auto node = std::make_shared<ShoreCommsNode>();
  node->init();
  rclcpp::spin(node);
  rclcpp::shutdown();
  return 0;
}
