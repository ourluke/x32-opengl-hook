#include <iostream>

#include "hooks.h"
#include "gui.h"

void shutdown(HINSTANCE instance, FILE* f)
{
    hooks::shutdownHooks();
    GUI::shutdown();

    if (f)
        fclose(f);

    FreeConsole();
    FreeLibraryAndExitThread(instance, 0);
}

DWORD WINAPI thread(LPVOID param)
{
    FILE* f = nullptr;

    AllocConsole();
    freopen_s(&f, "CONOUT$", "w", stdout);

    if (!hooks::init())
        shutdown(static_cast<HINSTANCE>(param), f);

    while (!GetAsyncKeyState(VK_END))
    {
        Sleep(100);
    }

    shutdown(static_cast<HINSTANCE>(param), f);
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

        HANDLE threadHandle = CreateThread(NULL, NULL, thread, instance, NULL, nullptr);

        if (threadHandle)
            CloseHandle(threadHandle);
    }

    return true;
}
