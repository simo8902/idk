//
// Created by Simeon on 4/27/2024.
//

#ifndef CORE_INITIALIZATION_H
#define CORE_INITIALIZATION_H
#include "Core.h"

class Initialization {
public:
    Initialization();
    ~Initialization();

    void runMainLoop() const;
private:
    IDK::Core& core;
    EngineAccess access;
};

#endif