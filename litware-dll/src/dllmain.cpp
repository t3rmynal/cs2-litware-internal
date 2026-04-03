#include "platform/winapi.h"
#include "debug.h"

HMODULE g_thisModule = nullptr;

void entry();

static DWORD WINAPI ThreadProc(LPVOID) {
    BootstrapLog("[litware] Thread started");
    DebugLog("[litware] Thread started");
    __try {
        entry();
    } __except (EXCEPTION_EXECUTE_HANDLER) {
        BootstrapLog("[litware] CRASH in entry() - code 0x%08lX", GetExceptionCode());
    }
    BootstrapLog("[litware] Thread finished");
    DebugLog("[litware] Thread finished");
    return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD reason, LPVOID lpReserved) {
    (void)lpReserved;

    switch (reason) {
    case DLL_PROCESS_ATTACH:
        BootstrapLog("[litware] DLL_PROCESS_ATTACH");
        DebugLog("[litware] DLL_PROCESS_ATTACH");
        g_thisModule = hModule;
        DisableThreadLibraryCalls(hModule);
        CreateThread(nullptr, 0, ThreadProc, nullptr, 0, nullptr);
        break;
    case DLL_PROCESS_DETACH:
        BootstrapLog("[litware] DLL_PROCESS_DETACH");
        DebugLog("[litware] DLL_PROCESS_DETACH");
        break;
    }
    return TRUE;
}
