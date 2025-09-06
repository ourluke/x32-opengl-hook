#include "hooks.h"

#include <iostream>

#include "external/minhook/MinHook.h"
#include "external/imgui/imgui_impl_win32.h"
#include "gui.h"
#include "globals.h"

typedef BOOL(WINAPI* SwapBuffers_t)(HDC hdc);
typedef int (__cdecl* SetRelativeMouseMode)(int mode);

SwapBuffers_t oSwapBuffers = nullptr;
SetRelativeMouseMode oSetRelativeMouseMode = nullptr;

WNDPROC oWndProc = nullptr;
HWND g_hWnd = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    if (globals::menu)
    {
        if (ImGui_ImplWin32_WndProcHandler(hWnd, msg, wParam, lParam))
            return true;

        switch (msg)
        {
        case WM_MOUSEMOVE:
        case WM_KEYUP:
        case WM_KEYDOWN:
        case WM_LBUTTONUP:
        case WM_LBUTTONDOWN:
        case WM_RBUTTONUP:
        case WM_RBUTTONDOWN:
        case WM_CHAR:
            return false;
        }
    }

    return CallWindowProc(oWndProc, hWnd, msg, wParam, lParam);
}

BOOL WINAPI hkSwapBuffers(HDC hdc)
{
    //static bool s_init = false;

    if (static bool s_init = false; !s_init)
    {
        g_hWnd = WindowFromDC(hdc);
        GUI::init(g_hWnd);

        oWndProc = (WNDPROC)SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)WndProc);
        s_init = true;
    }

    GUI::render();

    return oSwapBuffers(hdc);
}

bool hooks::init()
{
    void* pSwapBuffers = GetProcAddress(GetModuleHandle(L"gdi32.dll"), "SwapBuffers");
    oSetRelativeMouseMode = reinterpret_cast<SetRelativeMouseMode>(GetProcAddress(GetModuleHandle(L"sdl2.dll"), "SDL_SetRelativeMouseMode"));

    if (!pSwapBuffers || !oSetRelativeMouseMode)
        std::cout << "Failed to get SwapBuffers or SetRelativeMouseMode" << std::endl;

    if (MH_Initialize() != MH_OK)
        return false;

    if (MH_CreateHook(pSwapBuffers, &hkSwapBuffers, reinterpret_cast<void**>(&oSwapBuffers)) != MH_OK)
        return false;

    if (MH_EnableHook(pSwapBuffers) != MH_OK)
        return false;

    return true;
}

void hooks::shutdownHooks()
{
    MH_DisableHook(MH_ALL_HOOKS);
    MH_RemoveHook(MH_ALL_HOOKS);
    MH_Uninitialize();

    oSetRelativeMouseMode(1);

    SetWindowLongPtr(g_hWnd, GWLP_WNDPROC, (LONG_PTR)oWndProc);
}
