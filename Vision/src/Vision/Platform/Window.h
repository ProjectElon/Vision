#pragma once

#include "pch.hpp"

#include "Vision/Core/Defines.h"

namespace Vision
{
    enum class WindowMode
    {
        Windowed,
        Fullscreen
    };

	struct Window
	{
		void* Handle;
	};

	void OpenWindow(Window* window,
	                const char* title,
	                int32 xPosition,
	                int32 yPosition,
		  			uint32 width,
		  			uint32 height,
  		  			bool fullScreen = false);

	void CloseWindow(Window *window);
	void PumpEvents(Window *window);

	void SetWindowTitle(Window *window, const char* title);
	void SetWindowSize(Window *window, uint32 width, uint32 height);
	void SetWindowPosition(Window *window, int32 windowX, int32 windowY);

	void SetWindowMode(Window *window, bool fullScreen);
}