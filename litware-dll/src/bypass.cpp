#include "bypass.h"
#include "debug.h"
#include "platform/minhook_utils.h"
#include "platform/winapi.h"
#include <MinHook.h>
#include <cstdint>
#include <cstring>

namespace {

using BSecureAllowedFn = int (*)(char*, int, int);
static BSecureAllowedFn g_origBSecureAllowed = nullptr;
static bool g_bsecureHooked = false;
static bool g_secureFlagPatched = false;

static int HookBSecureAllowed(char* report_buf, int buf_size, int flags) {
    (void)report_buf;
    (void)buf_size;
    (void)flags;
    return 1;
}

static HMODULE ResolveExeModule() {
    HMODULE exe = GetModuleHandleA(nullptr);
    if (!exe) exe = GetModuleHandleA("cs2.exe");
    return exe;
}

static bool EnsureMinHook() {
    MH_STATUS st = MH_Initialize();
    if (st == MH_OK || st == MH_ERROR_ALREADY_INITIALIZED) return true;
    DebugLog("[bypass] MH_Initialize failed: %d", static_cast<int>(st));
    return false;
}

static void* PatternScan(HMODULE mod, const char* pat, const char* mask) {
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

static bool InstallBSecureAllowedHook(void* target) {
    if (!target) return false;
    if (g_bsecureHooked) return true;

    MH_STATUS st = minhook_utils::CreateHook(target, &HookBSecureAllowed, &g_origBSecureAllowed);
    if (st != MH_OK && st != MH_ERROR_ALREADY_CREATED) {
        DebugLog("[bypass] MH_CreateHook BSecureAllowed failed: %d", static_cast<int>(st));
        return false;
    }

    st = minhook_utils::EnableHook(target);
    if (st != MH_OK && st != MH_ERROR_ENABLED) {
        DebugLog("[bypass] MH_EnableHook BSecureAllowed failed: %d", static_cast<int>(st));
        return false;
    }

    g_bsecureHooked = true;
    DebugLog("[bypass] BSecureAllowed hooked OK");
    return true;
}

static bool PatchByte(uint8_t* addr, uint8_t expected, uint8_t value) {
    if (!addr) return false;

    __try {
        if (*addr == value) return true;
        if (*addr != expected) {
            DebugLog("[bypass] secure flag byte mismatch: got %u expected %u", static_cast<unsigned>(*addr), static_cast<unsigned>(expected));
            return false;
        }

        DWORD oldProt = 0;
        if (!VirtualProtect(addr, 1, PAGE_EXECUTE_READWRITE, &oldProt)) {
            DebugLog("[bypass] VirtualProtect failed: %lu", GetLastError());
            return false;
        }

        *addr = value;
        FlushInstructionCache(GetCurrentProcess(), addr, 1);
        VirtualProtect(addr, 1, oldProt, &oldProt);
        return true;
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        DebugLog("[bypass] PatchByte: exception");
        return false;
    }
}

}

namespace bypass {

bool Initialize() {
    HMODULE exe = ResolveExeModule();
    if (!exe) {
        DebugLog("[bypass] cs2.exe not found");
        return false;
    }

    void* pBSecureAllowed = reinterpret_cast<void*>(GetProcAddress(exe, "BSecureAllowed"));
    if (!pBSecureAllowed) {
        DebugLog("[bypass] BSecureAllowed export not found");
        return false;
    }

    if (!EnsureMinHook()) return false;
    return InstallBSecureAllowedHook(pBSecureAllowed);
}

void PatchSecureServerFlag() {
    if (g_secureFlagPatched) return;

    HMODULE client = GetModuleHandleA("client.dll");
    if (!client) {
        DebugLog("[bypass] client.dll not found");
        return;
    }

    static const char PAT[] = "\xC6\x05\x00\x00\x00\x00\x01";
    static const char MSK[] = "xx????x";
    void* hit = PatternScan(client, PAT, MSK);
    if (!hit) {
        DebugLog("[bypass] secure flag pattern not found");
        return;
    }

    uint8_t* instr = static_cast<uint8_t*>(hit);
    if (PatchByte(instr + 6, 1, 0)) {
        g_secureFlagPatched = true;
        DebugLog("[bypass] secure-server write patched");
    }
}

}
