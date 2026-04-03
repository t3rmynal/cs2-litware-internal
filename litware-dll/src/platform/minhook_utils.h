#pragma once

#include <MinHook.h>

namespace minhook_utils {

template<typename Target, typename Detour, typename Original>
inline MH_STATUS CreateHook(Target target, Detour detour, Original* original) {
    return MH_CreateHook(
        reinterpret_cast<LPVOID>(target),
        reinterpret_cast<LPVOID>(detour),
        reinterpret_cast<LPVOID*>(original)
    );
}

template<typename Target>
inline MH_STATUS EnableHook(Target target) {
    return MH_EnableHook(reinterpret_cast<LPVOID>(target));
}

}
