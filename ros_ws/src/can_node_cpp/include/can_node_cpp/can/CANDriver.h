//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include "CANDriver.h"
#include "MotorODParam.h"
#include "../SharedStore.h"

#include <iostream>
#include <lely/coapp/loop_driver.hpp>

#include "Motors.h"

namespace eboat {
    // CANDriver.h
    class CANDriver : public lely::canopen::LoopDriver {
    public:
        explicit CANDriver(SharedStore *shared_store, lely::canopen::AsyncMaster &master, const uint8_t id,
                           const Motors motor)
            : LoopDriver(master, id), motor_(motor), shared_store_(shared_store) {
        }

        using LoopDriver::LoopDriver;

        void enqueueReadSDO(const MotorODParam sdo_param) {
            bool need_start = false;
            {
                std::lock_guard lock(queue_mutex_);
                pending_.push(sdo_param);
                if (!worker_running_) {
                    worker_running_ = true;
                    need_start = true;
                }
            }
            if (need_start) {
                Post([this] { runWorker(); });
            }
        }

        void runWorker() {
            for (;;) {
                MotorODParam param{};
                {
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    if (pending_.empty()) {
                        worker_running_ = false;
                        return;
                    }
                    param = pending_.front();
                    pending_.pop();
                }
                switch (param.type) {
                    case ODType::U8: readAndStoreSDO<uint8_t>(param);
                        break;
                    case ODType::U16: readAndStoreSDO<uint16_t>(param);
                        break;
                    case ODType::U32: readAndStoreSDO<uint32_t>(param);
                        break;
                    case ODType::I8: readAndStoreSDO<int8_t>(param);
                        break;
                    case ODType::I16: readAndStoreSDO<int16_t>(param);
                        break;
                    case ODType::I32: readAndStoreSDO<int32_t>(param);
                        break;
                    default:
                        RCLCPP_ERROR(shared_store_->logger, "Unknown OD type in SDO Read for [%X:%d]", param.index,
                                     param.subindex);
                        break;
                }
            }
            worker_running_ = false;
        }

        template<typename T>
        void readAndStoreSDO(const MotorODParam &param) {
            try {
                auto value = Wait(AsyncRead<T>(param.index, param.subindex));
                shared_store_->store(this->motor_, param, CANData{value, std::chrono::system_clock::now()});
            } catch (const lely::canopen::SdoError &e) {
                RCLCPP_ERROR(shared_store_->logger, "SDO read failed for [%X:%d] - %s", param.index, param.subindex,
                             e.what());
            }
        }

        void ensureWorkerRunning() {
            if (worker_running_) return;
            worker_running_ = true;

            Post([this] { runWorker(); });
        }

        [[nodiscard]] uint16_t getQueueLength() {
            std::lock_guard lock(queue_mutex_);
            return static_cast<uint16_t>(pending_.size());
        }

    protected:
        void OnBoot(lely::canopen::NmtState, char, const std::string &) noexcept override {
            RCLCPP_INFO(shared_store_->logger, "[NMT] Motor %s has booted up!",
                        this->motor_ == Motors::MOTOR_A ? "A (ID 6)" : "B (ID 7)");
        }

        void OnRpdoWrite(uint16_t idx, uint8_t subidx) noexcept override {
            const MotorODParam *param = findParam(idx, subidx);
            if (!param) {
                RCLCPP_ERROR(shared_store_->logger, "RPDO [%X:%d] does not exist in known MotorOD Parameters.", idx,
                             subidx);
                return;
            }


            std::any value;
            dispatchType(param->type, [&]([[maybe_unused]] auto tag) {
                // this is to forcefully convert to the correct type
                using T = typename decltype(tag)::type;
                T v = rpdo_mapped[idx][subidx];
                value = v;
            });


            shared_store_->store(motor_, *param,
                                 CANData{.value = std::move(value), .timestamp = std::chrono::system_clock::now()});
        }

        void OnCanError(lely::io::CanError err) noexcept override {
            using lely::io::CanError;
            std::string error;
            switch (err) {
                // err is a lely::io::CanError
                case CanError::BIT:
                    error = "A single bit error.";
                    break;
                case CanError::STUFF:
                    error = "A bit stuffing error.";
                    break;
                case CanError::CRC:
                    error = "A CRC sequence error.";
                    break;
                case CanError::FORM:
                    error = "A form error.";
                    break;
                case CanError::ACK:
                    error = "An acknowledgment error.";
                    break;
                case CanError::OTHER:
                    error = "One or more other errors. This is all I get, seriously...";
                    break;
                case CanError::NONE:
                    error = "No error.";
                    break;
                default:
                    error = "Unknown error.";
                    break;
            }

            RCLCPP_ERROR(shared_store_->logger, "CAN Error Detected for Motor %s: %s",
                         this->motor_ == Motors::MOTOR_A ? "A (ID 6)" : "B (ID 7)", error.c_str());
        }

        void OnCanState(lely::io::CanState new_state, lely::io::CanState old_state) noexcept override {
            std::string oldStr;
            switch (old_state) {
                case lely::io::CanState::SLEEPING:
                    oldStr = "SLEEPING";
                    break;
                case lely::io::CanState::ACTIVE:
                    oldStr = "ACTIVE";
                    break;
                case lely::io::CanState::BUSOFF:
                    oldStr = "BUS OFF";
                    break;
                case lely::io::CanState::PASSIVE:
                    oldStr = "PASSIVE";
                    break;
                case lely::io::CanState::STOPPED:
                    oldStr = "STOPPED";
                    break;
            }
            std::string newStr;
            switch (new_state) {
                case lely::io::CanState::SLEEPING:
                    newStr = "SLEEPING";
                    break;
                case lely::io::CanState::ACTIVE:
                    newStr = "ACTIVE";
                    break;
                case lely::io::CanState::BUSOFF:
                    newStr = "BUS OFF";
                    break;
                case lely::io::CanState::PASSIVE:
                    newStr = "PASSIVE";
                    break;
                case lely::io::CanState::STOPPED:
                    newStr = "STOPPED";
                    break;
            }
            std::printf("State changed: %s to %s", oldStr.c_str(), newStr.c_str());
        }

    private:
        std::mutex queue_mutex_;
        std::queue<MotorODParam> pending_;
        bool worker_running_ = false;
        Motors motor_;
        SharedStore *shared_store_;
    };
} // namespace eboat
