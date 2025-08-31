#pragma once

#define WIN32_LEAN_AND_MEAN

#include <Windows.h>

namespace GUI
{
	void init(HWND hwnd);
	void render();
	void shutdown();
}