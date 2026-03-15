#pragma once

#include <vector>
#include <string>

// Bootstrap log - always writes to %TEMP%\litware_dll.log (for injection diagnosis)
void BootstrapLog(const char* fmt, ...);

// Debug logging (verbose) - always declared for linker, no-op when LITWARE_DEBUG undefined
void DebugLog(const char* fmt, ...);

// Access logs for in-game console
const std::vector<std::string>& GetDebugLogs();
void ClearDebugLogs();
