#pragma once
#include <cstdint>
#include "platform/winapi.h"

// прямой доступ к памяти

namespace memory {

inline uintptr_t GetModule(const char* name) {
    return reinterpret_cast<uintptr_t>(GetModuleHandleA(name));
}

template<typename T>
inline T Read(uintptr_t addr) {
    if (!addr) return T{};
    __try { return *reinterpret_cast<const T*>(addr); }
    __except(EXCEPTION_EXECUTE_HANDLER) { return T{}; }
}

template<typename T>
inline bool Write(uintptr_t addr, const T& value) {
    if (!addr) return false;
    __try { *reinterpret_cast<T*>(addr) = value; return true; }
    __except(EXCEPTION_EXECUTE_HANDLER) { return false; }
}

}
