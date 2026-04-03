#pragma once

#include <vector>
#include <string>

// пишет в %TEMP%\litware_dll.log
void BootstrapLog(const char* fmt, ...);

void DebugLog(const char* fmt, ...);

// логи для меню
std::vector<std::string> GetDebugLogs();
void ClearDebugLogs();
