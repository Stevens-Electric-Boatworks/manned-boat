//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once

namespace eboat {
class IState {
public:
  virtual ~IState() = default;

  /**
   * Performs a check to see if it is valid to switch to this state.
   *
   * @return True if this state is valid to be switched to
   */
  [[nodiscard]] virtual bool isValid() const = 0;

  /**
   * This function is called whenever this state becomes active. All
   * initilization should happen here.
   */
  virtual void onSwitch() const = 0;

  /**
   * This function is run periodically (e.g, every 10ms), and very active code
   * should run here.
   */
  virtual void periodic() const = 0;

  /**
   * This function will be called whenever the state will be switched, and any
   * cleanup is needed.
   */
  virtual void cleanup() const = 0;
};
} // namespace eboat
