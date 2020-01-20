#pragma once

#include "pch.h"

#include "Vision/Core/Core.h"
#include "Vision/Events/Event.h"

namespace Vision
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;

		WindowProps(const std::string& title = "Vision Engine",
					unsigned int width = 1280,
					unsigned int height = 720)
			: Title(title)
			, Width(width)
			, Height(height)
		{
		}
	};

	using EventCallbackFn = std::function<void(Event&)>;

	class Window
	{
	public:
		virtual ~Window() {}
		
		virtual void OnUpdate() = 0;
		
		virtual const std::string& GetTitle() = 0;
		virtual unsigned int GetWidth() = 0;
		virtual unsigned int GetHeight() = 0;

		virtual void SetEventCallback(const EventCallbackFn& fn) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual bool IsVSyncEnabled() const = 0;

		virtual void* GetNativeWindow() const = 0;

		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
	};
}