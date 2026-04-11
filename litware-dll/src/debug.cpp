#include "debug.h"
#include "platform/winapi.h"
#include <cstdarg>
#include <cstdio>
#include <ctime>

#include <mutex>
#include <deque>
#include <vector>
#include <string>

static std::deque<std::string> g_logs;
static std::mutex g_logMutex;
static FILE* g_logFile = nullptr;

static FILE* EnsureLogFile() {
    if (g_logFile) return g_logFile;
    char path[MAX_PATH];
    if (GetTempPathA(MAX_PATH, path) == 0) return nullptr;
    char fullPath[MAX_PATH];
    snprintf(fullPath, sizeof(fullPath), "%slitware_dll.log", path);
    fopen_s(&g_logFile, fullPath, "a");
    return g_logFile;
}

std::vector<std::string> GetDebugLogs() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    return {g_logs.begin(), g_logs.end()};
}

void ClearDebugLogs() {
    std::lock_guard<std::mutex> lock(g_logMutex);
    g_logs.clear();
}

void BootstrapLog(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    {
        std::lock_guard<std::mutex> lock(g_logMutex);
        g_logs.push_back(buf);
        if (g_logs.size() > 500) g_logs.pop_front();
    }

    FILE* f = EnsureLogFile();
    if (!f) return;
    time_t t; time(&t);
    struct tm lt; if (localtime_s(&lt, &t) == 0)
        fprintf(f, "[%02d:%02d:%02d] ", lt.tm_hour, lt.tm_min, lt.tm_sec);
    fprintf(f, "%s\n", buf);
    fflush(f);
}

void DebugLog(const char* fmt, ...) {
    char buf[1024];
    va_list args;
    va_start(args, fmt);
    vsnprintf(buf, sizeof(buf), fmt, args);
    va_end(args);

    {
        std::lock_guard<std::mutex> lock(g_logMutex);
        g_logs.push_back(buf);
        if (g_logs.size() > 500) g_logs.pop_front();
    }

#ifdef LITWARE_DEBUG
    FILE* f = EnsureLogFile();
    if (!f) return;
    fprintf(f, "%s\n", buf);
    fflush(f);
#endif
}
