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

	struct VideoMode
	{
		uint32 Width;
		uint32 Height;
		uint32 RefreshRate;
	};

	struct WindowData
	{
		std::string 	Title;
		uint32      	Width;
		uint32      	Height;
		int32 	    	WindowX;
		int32	    	WindowY;
		WindowMode      Mode;
		bool            VSync;
		bool			Maximized;
		EventCallbackFn EventCallback;
		
		WindowData()
			: Title("Untitled")
			, Width(1280)
			, Height(720)
			, WindowX(0)
			, WindowY(0)
			, VSync(false)
			, Maximized(false)
			, Mode(WindowMode::Windowed)
		{
		}
	};

	class Window
	{
	public:
		virtual ~Window() {}
		
		virtual void OnUpdate() = 0;
		
		virtual void SetEventCallback(const EventCallbackFn& eventCallback) = 0;
		
		virtual void SetTitle(const char* title) = 0;
		virtual void SetSize(uint32 width, uint32 height) = 0;
		virtual void SetMode(WindowMode windowMode) = 0;
		virtual void SetVSync(bool enabled) = 0;
		virtual void SetPosition(int32 windowX, int32 windowY) = 0;

		virtual void Maximize() = 0;
		virtual void Restore() = 0;

		virtual const WindowData& GetData() const = 0;
		
		virtual void* GetNativeHandle() const = 0;
		virtual std::vector<VideoMode> GetMonitorVideoModes() const = 0;

		static Scope<Window> Create(const WindowData& data = WindowData());
	};
}