#pragma once
#include <cstdint>
#include "platform/winapi.h"

namespace interfaces {
    extern uintptr_t client;
    extern uintptr_t engine;
    extern uintptr_t entityList;
    extern uintptr_t localPlayerController;
    extern uintptr_t localPlayerPawn;
    extern uintptr_t csgoInput;
    
    bool Initialize();
}
