//
// Created by SIMEON on 4/5/2025.
//
#include "Core.h"

namespace IDK
{
    Core::Core()
    {
        IDK::EngineSystems::Config cfg;

        engineSystems = std::make_unique<IDK::EngineSystems>(cfg);
    }

    Core::~Core() = default;

    Core& Core::Instance()
    {
        static Core instance;
        return instance;
    }
}