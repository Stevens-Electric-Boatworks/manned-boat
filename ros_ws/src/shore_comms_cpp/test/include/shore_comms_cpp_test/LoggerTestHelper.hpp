//
// Created by ishaan on 11/17/25.
//

#pragma once
#include <memory>

#include "mock/MockDataReceiver.hpp"
#include "mock/MockDataTransmitter.hpp"

class MockDataTransmitter;

template <typename MsgT, typename LoggerT>
struct LoggerTestHelper {
    using RecT = MockDataReceiver<MsgT>;

    std::shared_ptr<RecT> rec;
    std::shared_ptr<MockDataTransmitter> transmitter;
    std::unique_ptr<LoggerT> logger;

    template<typename... Args>
    explicit LoggerTestHelper(Args&&... args) {
        rec = std::make_shared<RecT>();
        transmitter = std::make_shared<MockDataTransmitter>();
        logger = std::make_unique<LoggerT>(rec, transmitter, std::forward<Args>(args)...);
    }
};
