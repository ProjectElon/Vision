#pragma once

#include "pch.h"

#include "Vision/Core/Core.h"
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
		std::string Title = "Vision Engine";
		uint32_t Width = 1280;
		uint32_t Height = 720;
		bool VSync = false;
		WindowMode Mode = WindowMode::Windowed;
		uint32_t RedBits = 8;
		uint32_t GreenBits = 8;
		uint32_t BlueBits = 8;
		uint32_t AlphaBits = 8;
		uint32_t DepthBits = 24;
		uint32_t StencilBits = 8;
		uint32_t Samples = 0;
		uint32_t BufferCount = 2;
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

		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
	};
}