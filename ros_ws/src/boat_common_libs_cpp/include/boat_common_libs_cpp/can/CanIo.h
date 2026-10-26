//
// Created by Ishaan Sayal.
// Copyright (c) 2026 Stevens Electric Boatworks.

#pragma once
#include <string>
#include <utility>

namespace eboat::can
{
    class CanIo
    {
    public:
        explicit CanIo(std::string  interface_name)
            : interface_name(std::move(interface_name))
        {
        }

    private:
        std::string interface_name;

    };
}
