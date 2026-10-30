//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <atomic>
#include <functional>
#include <string>
#include <thread>
#include <utility>

#include "sockcanpp/CanDriver.hpp"

namespace eboat::can
{
    /**
     * Manages the underlying IO interface of the CAN bus using libsockcanpp
     */
    class CanIo
    {
    public:
        explicit CanIo(std::string  interface_name)
            : interface_name(std::move(interface_name))
        {
            driver = std::make_unique<sockcanpp::CanDriver>(this->interface_name, CAN_RAW);
            driver->setDefaultSenderId(sockcanpp::CanId{0x01});
            driver->setErrorFilter(true);
        }
        ~CanIo();
        CanIo(const CanIo&) = delete;
        CanIo& operator=(const CanIo&) = delete;
        typedef std::function<void(sockcanpp::CanMessage msg)> msg_callback;
        void register_msg_callback(const msg_callback& func);
        void register_error_msg_callback(const msg_callback& func);
        void start_recv_thread();
        void shutdown();
    private:
        void handleErrorFrame(const sockcanpp::CanMessage& msg) const;
        void handleFrame(const sockcanpp::CanMessage& msg) const;


        std::vector<msg_callback> msg_callbacks = {};
        std::vector<msg_callback> error_msg_callbacks = {};
        std::thread msg_poll_thread;
        std::string interface_name;
        std::unique_ptr<sockcanpp::CanDriver> driver;
        std::atomic<bool> active = false;
    };
}
