#include "can_node_cpp/state_lib/StateManager.h"

#include <memory>
#include <string>

#include "rclcpp/rclcpp.hpp"

#include <fmt/core.h>

using namespace std::chrono_literals;
class CANNode : public rclcpp::Node {
public:
  CANNode() : Node("can_node_cpp") {
    std::cout << "starting the timers" << "\n";
    if (this->_stateManager == nullptr) {
      std::cout << "Making new shared instance" << "\n";
      this->_stateManager = std::make_shared<eboat::StateManager>();
    }
    std::cout << "Initializing" << "\n";
    _stateManager->initialize();
    std::cout << "Initializing finished" << "\n";
    std::cout << "Going to run periodic from thw wall timer now" << "\n";
    _stateManager->tickBus();
    _stateManager->tickPeriodic();

    // this->timer_ = this->create_wall_timer(500ms, [this]() -> void {
    //
    // });
    // this->can_timer_ = this->create_wall_timer(100ms, [this]() -> void {
    //   if (this->_stateManager == nullptr) {
    //     //wait until it exists
    //     std::cout << "state manager is null" << "\n";
    //   }
    //   else {
    //     std::cout << "ticking the can bus" << "\n";
    //   }
    // });
  }

private:
  rclcpp::TimerBase::SharedPtr timer_;
  rclcpp::TimerBase::SharedPtr can_timer_;
  std::shared_ptr<eboat::StateManager> _stateManager;
};

int main(int argc, char *argv[]) {
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<CANNode>());
  rclcpp::shutdown();
  return 0;
}