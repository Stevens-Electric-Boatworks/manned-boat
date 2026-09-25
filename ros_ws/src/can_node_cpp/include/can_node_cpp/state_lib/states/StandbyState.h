//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "../IState.h"
#include "States.h"

#include <functional>

namespace eboat {
class CANBusService;
class StandbyState : public IState {
  CANBusService& busService;
  std::function<void(States)> switchTo;
public:
  explicit StandbyState(CANBusService& canBus,
                        const std::function<void(States)>& switchTo)
      : busService(canBus), switchTo(switchTo) {};
  ~StandbyState() override;
  [[nodiscard]] bool isValid() const override;
  void onSwitch() const override;
  void periodic() const override;
  void cleanup() const override;
};
} // namespace eboat