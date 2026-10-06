//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "../../include/can_node_cpp/SharedStore.h"

#include "can_node_cpp/can/Motors.h"

std::optional<eboat::CANData> eboat::SharedStore::getSDO(const Motors motor, const MotorODParam param) const {
    this->get_queued_reads().push(param);
    if (const auto it = this->data->find(motor); it != this->data->end()) {
        if (const auto it2 = it->second.find(param); it2 != it->second.end()) {
            return it2->second;
        }
    }
    return std::nullopt;
}

void eboat::SharedStore::store(const Motors motor, const MotorODParam param, CANData canData) const {
    if (const auto it = data->find(motor); it != data->end()) {
        it->second.insert_or_assign(param, canData);
    } else {
        std::unordered_map<MotorODParam, CANData> map = {{param, canData}};
        data->insert_or_assign(motor, map);
    }

    if (this->onCANDataReceive != nullptr) {
        onCANDataReceive(motor, param, canData);
    }
}
