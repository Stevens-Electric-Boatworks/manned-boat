//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

#include "CANDriver.h"
#include "MotorSDOParam.h"
#include "../SharedStore.h"

#include <iostream>
#include <lely/coapp/loop_driver.hpp>

namespace eboat
{
    // CANDriver.h
    class CANDriver : public lely::canopen::LoopDriver
    {
    public:
        using LoopDriver::LoopDriver;

        void enqueueReadSDO(SharedStore& shared_store, MotorSDOParam sdo_param)
        {
            bool need_start = false;
            {
                std::lock_guard<std::mutex> lock(queue_mutex_);
                pending_.push(sdo_param);
                if (!worker_running_)
                {
                    worker_running_ = true;
                    need_start = true;
                }
            }
            if (need_start)
            {
                // std::cout << "New worker" << std::endl;
                Post([this, &shared_store] { runWorker(shared_store); });
            }
        }

        void runWorker(SharedStore& shared_store)
        {
            for (;;)
            {
                MotorSDOParam param;
                {
                    std::lock_guard<std::mutex> lock(queue_mutex_);
                    if (pending_.empty())
                    {
                        worker_running_ = false;
                        return;
                    }
                    param = pending_.front();
                    pending_.pop();
                }
                switch (param.type)
                {
                case SDOType::U8: readOne<uint8_t>(shared_store, param);
                    break;
                case SDOType::U16: readOne<uint16_t>(shared_store, param);
                    break;
                case SDOType::U32: readOne<uint32_t>(shared_store, param);
                    break;
                case SDOType::I8: readOne<int8_t>(shared_store, param);
                    break;
                case SDOType::I16: readOne<int16_t>(shared_store, param);
                    break;
                case SDOType::I32: readOne<int32_t>(shared_store, param);
                    break;
                }
            }
            worker_running_ = false;
        }

        template <typename T>
        void readOne(SharedStore& shared_store, const MotorSDOParam& param)
        {
            try
            {
                auto value = Wait(AsyncRead<T>(param.index, param.subindex));
                shared_store.storeSDO(param, CANData{value, std::chrono::system_clock::now()});
            }
            catch (const lely::canopen::SdoError& e)
            {
                std::cerr << "SDO read failed for " << std::hex << param.index
                    << ":" << +param.subindex << " — " << e.what() << "\n";
            }
        }

        void ensureWorkerRunning(SharedStore& shared_store)
        {
            if (worker_running_) return;
            worker_running_ = true;

            Post([this, &shared_store] { runWorker(shared_store); });
        }

    private:
        void OnBoot(lely::canopen::NmtState st, char es, const std::string& what) noexcept override
        {
            std::printf("Boot triggererd!\n");
        }

    private:
        std::mutex queue_mutex_;
        std::queue<MotorSDOParam> pending_;
        bool worker_running_ = false;
    };
} // namespace eboat
