#include "gui.h"

#include "external/imgui/imgui.h"
#include "external/imgui/imgui_impl_win32.h"
#include "external/imgui/imgui_impl_opengl2.h"

#include "globals.h"

typedef int(__cdecl* SetRelativeMouseMode)(int mode);
extern SetRelativeMouseMode oSetRelativeMouseMode;


void GUI::init(HWND hwnd)
{
	ImGui::CreateContext();
	ImGui_ImplWin32_Init(hwnd);
	ImGui_ImplOpenGL2_Init();
}

void GUI::render()
{
    ImGui_ImplOpenGL2_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (GetAsyncKeyState(VK_INSERT) & 1)
    {
        globals::menu = !globals::menu;
        oSetRelativeMouseMode(!globals::menu);
    }

    if (globals::menu)
    {
        ImGui::Begin("menu");
        ImGui::Text("hey");
        static bool checkbox = false;
        ImGui::Checkbox("test", &checkbox);
        ImGui::End();
    }

    ImGui::Render();
    ImGui_ImplOpenGL2_RenderDrawData(ImGui::GetDrawData());
}

void GUI::shutdown()
{
    ImGui_ImplOpenGL2_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}