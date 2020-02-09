#pragma once

#include "Vision/Platform/Window.h"
#include "Vision/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	struct WindowData
	{
		std::string Title;
		WindowMode Mode;
		unsigned int Width;
		unsigned int Height;
		int XPosition;
		int YPosition;
		bool VSync;
		EventCallbackFn EventCallback;
	};

	class DesktopWindow : public Window
	{
	public:
		DesktopWindow(const WindowProps& props);
		virtual ~DesktopWindow();

		void SetMode(WindowMode mode) override;
		void SetVSync(bool enabled) override;
		
		inline void SetEventCallback(const EventCallbackFn& fn) override { m_Data.EventCallback = fn; }

		void OnUpdate() override;

		inline void SetTitle(const char* title) 
		{
			m_Data.Title = title;
			glfwSetWindowTitle(m_WindowHandle, title);
		}
		
		inline void SetSize(unsigned int width, unsigned int height)
		{ 
			m_Data.Width = width; 
			m_Data.Height = height;
			glfwSetWindowSize(m_WindowHandle, width, height);
		}
		
		inline void SetPosition(int windowX, int windowY) 
		{
			m_Data.XPosition = windowX;
			m_Data.YPosition = windowY;
			glfwSetWindowPos(m_WindowHandle, windowX, windowY);
		}

		inline WindowMode GetMode() const override { return m_Data.Mode; }

		inline void* GetNativeWindowHandle() const override { return m_WindowHandle; }

		inline bool IsVSyncEnabled() const override { return m_Data.VSync; }

		inline const std::string& GetTitle() const override { return m_Data.Title; }

		inline unsigned int GetWidth() const override { return m_Data.Width; }
		
		inline unsigned int GetHeight() const override {  return m_Data.Height; }

		inline std::pair<unsigned int, unsigned int> GetSize() const override { return { m_Data.Width, m_Data.Height }; }

		inline std::pair<int, int> GetPosition() const override { return { m_Data.XPosition, m_Data.YPosition }; }
		
		std::vector<VideoMode> GetMonitorVideoModes() const override;

	private:
		GLFWwindow* m_WindowHandle;
		WindowData m_Data;

		std::unique_ptr<GraphicsContext> m_GraphicsContext;
	};
}