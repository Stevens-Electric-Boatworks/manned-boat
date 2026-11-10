//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "boat_common_libs_cpp/can/CanIo.h"

#include <iostream>

eboat::can::CanIo::~CanIo()
{
    shutdown();
}

void eboat::can::CanIo::register_msg_callback(const msg_callback& func)
{
    this->msg_callbacks.push_back(func);
}

void eboat::can::CanIo::register_error_msg_callback(const msg_callback& func)
{
    this->error_msg_callbacks.push_back(func);
}

void eboat::can::CanIo::start_recv_thread()
{
    this->active = true;
    this->msg_poll_thread = std::thread([this]
    {
        while (this->active)
        {
            try
            {
                if (!this->driver->waitForMessages(std::chrono::milliseconds(20)))
                {
                    continue;
                }
                if (const auto msg = driver->readMessage(); msg.isErrorFrame())
                {
                    handleErrorFrame(msg);
                }
                else
                {
                    handleFrame(msg);
                }
            }
            catch (std::exception& e)
            {
                std::cerr << "CAN receive error: " << e.what() << '\n'; //TODO: add proper ROS logging
                std::this_thread::sleep_for(std::chrono::milliseconds(50));
            }
        }
    });
}

void eboat::can::CanIo::shutdown()
{
    this->active = false;
    if (this->msg_poll_thread.joinable())
    {
        this->msg_poll_thread.join();
    }
}

void eboat::can::CanIo::handleErrorFrame(const sockcanpp::CanMessage& msg) const
{
    for (const auto& msg_callback : this->error_msg_callbacks)
    {
        msg_callback(msg);
    }
}

void eboat::can::CanIo::handleFrame(const sockcanpp::CanMessage& msg) const
{
    // TODO: Proper logging of all CAN bus frames
    // std::printf("We got a can frame: ID: %d, Data:", *msg.getCanId());
    // for (unsigned char c : msg.getFrameData()) {
    //     std::printf("0x%02X ", c);
    // }
    // std::printf("\n");
    for (const auto& msg_callback : this->msg_callbacks)
    {
        msg_callback(msg);
    }
}
