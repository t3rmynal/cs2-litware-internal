#pragma once
#include "../platform/winapi.h"
#include <cstdint>
#include <cstring>

inline void* PatternScan(HMODULE mod, const char* pat, const char* mask) {
    if (!mod || !pat || !mask) return nullptr;
    MODULEINFO mi{};
    if (!GetModuleInformation(GetCurrentProcess(), mod, &mi, sizeof(mi))) return nullptr;
    auto* b = static_cast<const uint8_t*>(mi.lpBaseOfDll);
    const size_t sz = mi.SizeOfImage;
    const size_t pl = std::strlen(mask);
    for (size_t i = 0; i + pl <= sz; ++i) {
        bool ok = true;
        for (size_t j = 0; j < pl && ok; ++j)
            if (mask[j] != '?' && b[i + j] != static_cast<uint8_t>(pat[j]))
                ok = false;
        if (ok) return const_cast<void*>(static_cast<const void*>(b + i));
    }
    return nullptr;
}
