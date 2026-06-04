//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include "../IState.h"

namespace eboat {
class IdleState : public IState {
public:
  ~IdleState() override;
  [[nodiscard]] bool isValid() const override;
  void onSwitch() const override;
  void periodic() const override;
  void cleanup() const override;
};
} // namespace eboat