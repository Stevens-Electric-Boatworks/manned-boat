//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.


#include <memory>
#include <string>

#include <boat_common_libs_cpp/can/CanDeviceManager.h>
#include <boat_common_libs_cpp/can/devices/eboat/ExampleCanNode.h>
#include <rclcpp/executors.hpp>
#include <rclcpp/node.hpp>

#include "boat_common_libs_cpp/can/CanIo.h"

using namespace std::chrono_literals;

/* This example creates a subclass of Node and uses a fancy C++11 lambda
* function to shorten the callback syntax, at the expense of making the
* code somewhat more difficult to understand at first glance. */

namespace eboat
{
    class ROSCanNode : public rclcpp::Node
    {
    public:
        ROSCanNode()
            : Node("cpp_can_node"),
              io_("vcan0"),
              device_manager_(io_)
        {
            device_manager_.register_device(std::make_unique<can::device::ExampleCanNode>(0x10));

            io_.start_recv_thread();
        }

    private:
        // io_ must be declared before device_manager_, which holds a reference to it
        can::CanIo io_;
        can::CanDeviceManager device_manager_;
    };
}

int main(const int argc, char * argv[])
{
    rclcpp::init(argc, argv);
    rclcpp::spin(std::make_shared<eboat::ROSCanNode>());
    rclcpp::shutdown();
    return 0;
}