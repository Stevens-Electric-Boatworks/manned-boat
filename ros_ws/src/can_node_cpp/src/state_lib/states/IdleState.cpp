//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#include "can_node_cpp/state_lib/states/IdleState.h"
eboat::IdleState::~IdleState() = default;
bool eboat::IdleState::isValid() const {
  //TODO implement
  return false;
}
void eboat::IdleState::onSwitch() const {}
void eboat::IdleState::periodic() const {}
void eboat::IdleState::cleanup() const {}
