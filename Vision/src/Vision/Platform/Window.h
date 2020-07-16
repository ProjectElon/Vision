#pragma once

#include "pch.h"

#include "Vision/Core/Base.h"
#include "Vision/Events/Event.h"

namespace Vision
{
	enum class WindowMode
	{
		Windowed,
		Fullscreen
	};

	struct VideoMode
	{
		uint32_t Width;
		uint32_t Height;
		uint32_t RefreshRate;
	};

	struct WindowProps
	{
		std::string Title;
		uint32_t Width;
		uint32_t Height;
		bool VSync;
		WindowMode Mode;
		uint32_t RedBits;
		uint32_t GreenBits;
		uint32_t BlueBits;
		uint32_t AlphaBits;
		uint32_t DepthBits;
		uint32_t StencilBits;
		
		WindowProps()
			: Title("Vision Engine")
			, Width(1280)
			, Height(720)
			, VSync(false)
			, Mode(WindowMode::Windowed)
			, RedBits(8)
			, GreenBits(8)
			, BlueBits(8)
			, AlphaBits(8)
			, DepthBits(24)
			, StencilBits(8)
		{
		}
	};

	using EventCallbackFn = std::function<void(Event&)>;

	class Window
	{
	public:
		virtual ~Window() {}
		
		virtual void OnUpdate() = 0;
		
		virtual void SetEventCallback(const EventCallbackFn& fn) = 0;
		virtual void SetMode(WindowMode windowMode) = 0;

		virtual void SetVSync(bool enabled) = 0;

		virtual void SetTitle(const char* title) = 0;
		virtual void SetSize(uint32_t width, uint32_t height) = 0;
		virtual void SetPosition(int windowX, int windowY) = 0;

		virtual WindowMode GetMode() const = 0;
		virtual bool IsVSyncEnabled() const = 0;
		virtual void* GetNativeWindowHandle() const = 0;

		virtual const std::string& GetTitle() const = 0;
		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;
		virtual std::pair<uint32_t, uint32_t> GetSize() const = 0;
		virtual std::pair<int, int> GetPosition() const = 0;
		
		virtual std::vector<VideoMode> GetMonitorVideoModes() const = 0;

		static Scope<Window> Create(const WindowProps& props = WindowProps());
	};
}