//
// Created by ishaan on 11/17/25.
//

#pragma once
#include <memory>

#include "mock/FakeDataReceiver.hpp"
#include "mock/FakeDataTransmitter.hpp"

class FakeDataTransmitter;

template <typename MsgT, typename LoggerT>
struct LoggerTestHelper {
    using RecT = FakeDataReceiver<MsgT>;

    std::shared_ptr<RecT> rec;
    std::shared_ptr<FakeDataTransmitter> transmitter;
    std::unique_ptr<LoggerT> logger;

    template<typename... Args>
    explicit LoggerTestHelper(Args&&... args) {
        rec = std::make_shared<RecT>();
        transmitter = std::make_shared<FakeDataTransmitter>();
        logger = std::make_unique<LoggerT>(rec, transmitter, std::forward<Args>(args)...);
    }
};
