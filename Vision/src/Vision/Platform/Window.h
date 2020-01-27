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

	struct WindowProps
	{
		std::string Title = "Vision Engine";
		unsigned int Width = 1280;
		unsigned int Height = 720;
		bool VSync = false;
		WindowMode Mode = WindowMode::Windowed;
		unsigned int RedBits = 8;
		unsigned int GreenBits = 8;
		unsigned int BlueBits = 8;
		unsigned int AlphaBits = 8;
		unsigned int DepthBits = 24;
		unsigned int StencilBits = 8;
		unsigned int Samples = 0;
		unsigned int BufferCount = 2;
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
		virtual void SetSize(unsigned int width, unsigned int height) = 0;
		virtual void SetPosition(int windowX, int windowY) = 0;

		virtual WindowMode GetMode() const = 0;
		virtual bool IsVSyncEnabled() const = 0;
		virtual void* GetNativeWindowHandle() const = 0;

		virtual const std::string& GetTitle() const = 0;
		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual std::pair<unsigned int, unsigned int> GetSize() const = 0;
		virtual std::pair<int, int> GetPosition() const = 0;
		
		static std::unique_ptr<Window> Create(const WindowProps& props = WindowProps());
	};
}