# Меню и конфиг (LitWare)

## Меню (ImGui)

- Файл: `litware-dll/src/hooks/render_hook.cpp` — отрисовка оверлея в `Present`, кастомные виджеты (`PidoTab`, `PidoToggle`, `PidoSliderFloat`, …), группы `BeginPidoGroup` (Aimbot, ESP, Visuals, Config и т.д.).
- Вкладки и секции UI напрямую читают/пишут глобальные флаги и значения, которые те же функции сохраняют в конфиг.

## Конфиги

- Каталог: `%APPDATA%\litware\`, файлы **`*.cfg`** (текст `ключ=значение`).
- Сохранение/загрузка: `SaveConfig` / `LoadConfig` в `render_hook.cpp`; внизу меню — блок **Config** (имя, список, Save/Load). 
- Разбор по категориям: `LoadConfigKeyEsp`, `LoadConfigKeyChams`, `LoadConfigKeyAimbot`, `LoadConfigKeyMovement`, `LoadConfigKeyVisual`, `LoadConfigKeySkins`, `LoadConfigKeyMisc`.

## Рефакторинг

Логику загрузки ключей можно вынести в отдельные `.cpp`, добавив единицы компиляции в `litware-dll/litware-dll.vcxproj`.

