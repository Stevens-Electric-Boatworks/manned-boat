//
// Created by ishaan on 11/17/25.
//

#include "shore_comms_cpp_test/mock/MockDataTransmitter.hpp"

void MockDataTransmitter::send_alarm(const Alarm &alarm) {
}

void MockDataTransmitter::send_can_bus_state(const uint8_t &can_state) {
}

void MockDataTransmitter::send_data(const nlohmann::json &json) {
    this->data.merge_patch(json);
}

void MockDataTransmitter::send_log(const LogData &log_data) {
}

bool MockDataTransmitter::has(const nlohmann::json &json) const {
    printf("Data: %s", data.dump().c_str());
    printf("Json: %s", json.dump().c_str());

    if (!json.is_object() || ! data.is_object()) return false;

    for (auto& [key, val] : json.items()) {
        if (!data.contains(key)) return false;
        if (data[key] != val) return false;
    }

    return true;

}
