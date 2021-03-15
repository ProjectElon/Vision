#pragma once

#include "pch.h"

#include "Vision/Core/Common.h"
#include "Vision/Events/Event.h"

namespace Vision
{
	using EventCallbackFn = std::function<void(Event&)>;

    enum class WindowMode
    {
        Windowed,
        Fullscreen
    };

	struct Settings;

	struct Window
	{
		void*	  Handle;
		Settings* Settings;

		EventCallbackFn EventCallback = nullptr;

		void Open(const char* title,
				  uint32 width,
				  uint32 height,
	  			  EventCallbackFn eventCallback,
		  		  bool fullScreen = false,
	  			  int32 windowX = 0,
	  			  int32 windowY = 0);

		void Close();
		void PollEvents();

		void SetTitle(const char* title);
		void SetSize(uint32 width, uint32 height);
		void SetPosition(int32 windowX, int32 windowY);

		void Maximize();
		void Minimize();
		void Restore();

		void SetMode(bool fullScreen);
	};
}