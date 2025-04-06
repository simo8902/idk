//
// Created by Simeon on 4/27/2024.
//

#include "Initialization.h"
#include "EngineAccess.h"

Initialization::Initialization() : core(IDK::Core::Instance()),
      access(core.getAccess())
{}

Initialization::~Initialization(){}

void Initialization::runMainLoop() const
{
    core.systems().runMainLoop();
}

