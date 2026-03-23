#pragma once

namespace bypass {

// лучше вызывать в entry пораньше
bool Initialize();

// вызывать после загрузки игры
void PatchSecureServerFlag();

}
