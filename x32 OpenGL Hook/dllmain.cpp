#include <iostream>

#include "hooks.h"
#include "gui.h"

void shutdown(HMODULE instance)
{
    hooks::shutdownHooks();
    GUI::shutdown();
    FreeConsole();
    FreeLibraryAndExitThread(instance, 0);
}

DWORD WINAPI thread(HMODULE instance)
{
    FILE* f;

    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);
    freopen_s(&f, "CONIN$", "r", stdin);

    if (!hooks::init())
        shutdown(instance);

    while (!GetAsyncKeyState(VK_END))
    {
        Sleep(100);
    }

    shutdown(instance);
    return 0;
}

bool WINAPI DllMain(
    HINSTANCE instance,
    std::uintptr_t reason,
    void* reserved)
{
    if (reason == DLL_PROCESS_ATTACH)
    {
        DisableThreadLibraryCalls(instance);

        HANDLE threadHandle = CreateThread(NULL, NULL, reinterpret_cast<LPTHREAD_START_ROUTINE>(thread), instance, NULL, nullptr);

        if (threadHandle)
            CloseHandle(threadHandle);
    }

    return true;
}