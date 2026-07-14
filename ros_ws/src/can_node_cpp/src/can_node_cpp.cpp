#include "can_node_cpp/monitor/MonitorLoop.h"
#include "can_node_cpp/state_lib/ControlLoop.h"

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include <fmt/core.h>

using namespace std::chrono_literals;
class CANNode : public rclcpp::Node {
public:
  CANNode() : Node("can_node_cpp") {
    this->_controlLoop = std::make_shared<eboat::ControlLoop>();
    this->_controlLoop->initialize();
    this->_monitorLoop = std::make_shared<eboat::MonitorLoop>(*this->_controlLoop->canBus);
    this->monitoring_Loop_Timer  = create_wall_timer(20ms, [this]() ->  void {
      this->_monitorLoop->tick();
    });
  }

private:
  rclcpp::TimerBase::SharedPtr monitoring_Loop_Timer;
  rclcpp::TimerBase::SharedPtr lely_timer;
  std::shared_ptr<eboat::MonitorLoop> _monitorLoop;
  std::shared_ptr<eboat::ControlLoop> _controlLoop;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CANNode>());
  rclcpp::shutdown();
  return 0;
}