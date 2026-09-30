#include "can_node_cpp/monitor/MonitorLoop.h"
#include "can_node_cpp/state_lib/ControlLoop.h"

#include <memory>
#include <string>
#include "std_msgs/msg/int16.hpp"
#include "rclcpp/rclcpp.hpp"

using namespace std::chrono_literals;
namespace
{
  class CANNode : public rclcpp::Node {
  public:
    CANNode() : Node("can_node_cpp") {
      this->publisher_ = this->create_publisher<std_msgs::msg::Int16>("topic", 10);
      this->_controlLoop = std::make_shared<eboat::ControlLoop>();
      this->_controlLoop->initialize(this);
      this->_monitorLoop = std::make_shared<eboat::MonitorLoop>(*this->_controlLoop->canBus);

      this->monitoring_Loop_Timer  = create_wall_timer(10ms, [this]() ->  void {
        this->_monitorLoop->tick();
      });
      this->control_loop_timer  = create_wall_timer(20ms, [this]() ->  void {
        this->_controlLoop->tickPeriodic();
      });
      auto ref = _monitorLoop->can_bus_service.loop.get();

      this->thread = std::make_shared<std::thread>([&ref]()->  void {
        ref->run();
      });

    }

  private:
    rclcpp::TimerBase::SharedPtr monitoring_Loop_Timer;
    rclcpp::TimerBase::SharedPtr control_loop_timer;
    std::shared_ptr<eboat::MonitorLoop> _monitorLoop;
    std::shared_ptr<eboat::ControlLoop> _controlLoop;
    rclcpp::Publisher<std_msgs::msg::Int16>::SharedPtr publisher_;
    std::shared_ptr<std::thread> thread;

  };
}

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CANNode>());
  rclcpp::shutdown();
  return 0;
}