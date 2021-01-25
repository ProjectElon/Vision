#pragma once

#include "pch.h"

#include "Vision/Core/Base.h"
#include "Vision/Events/Event.h"

namespace Vision
{
	using EventCallbackFn = std::function<void(Event&)>;

	enum class WindowMode
	{
		Windowed,
		Fullscreen
	};

	struct Window
	{
		void* Handle;

		std::string Title = "Untitled";

		uint32 Width  = 1280;
		uint32 Height = 720;

		int32 WindowX = 0;
		int32 WindowY = 0;

		//@(Harlequin): WindowFlags for Mode and Maximized
		WindowMode Mode = WindowMode::Windowed;

		bool Maximized = true;

		EventCallbackFn EventCallback = nullptr;
	};

	void OpenWindow(Window* window,
					const std::string& title,
					uint32 width,
					uint32 height,
					EventCallbackFn eventCallback,
					WindowMode mode = WindowMode::Windowed,
					int32 windowX = 0,
					int32 windowY = 0);

	void CloseWindow(Window* window);

	void PollEvents();

	void SetWindowTitle(Window* window,
						const std::string& title);

	void SetWindowSize(Window* window,
					   uint32 width,
					   uint32 height);

	void SetWindowPosition(Window* window,
						   int32 windowX,
						   int32 windowY);

	void MaximizeWindow(Window* window);
	void MinimizeWindow(Window* window);
	void RestoreWindow(Window* window);

	void SetWindowMode(Window* window, WindowMode mode);
}