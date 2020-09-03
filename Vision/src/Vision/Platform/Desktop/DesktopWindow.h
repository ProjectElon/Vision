#pragma once

#include "Vision/Platform/Window.h"
#include "Vision/Renderer/GraphicsContext.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	class DesktopWindow : public Window
	{
	public:
		DesktopWindow(const WindowData& data);
		virtual ~DesktopWindow();

		void OnUpdate() override;

		inline void SetEventCallback(const EventCallbackFn& eventCallback) override 
		{ 
			m_WindowData.EventCallback = eventCallback;
		}

		inline void SetTitle(const char* title) override
		{
			m_WindowData.Title = title;
			glfwSetWindowTitle(m_WindowHandle, title);
		}

		inline void SetSize(unsigned int width, unsigned int height) override
		{ 
			m_WindowData.Width = width;
			m_WindowData.Height = height;
			glfwSetWindowSize(m_WindowHandle, width, height);
		}

		void SetMode(WindowMode mode) override;
		void SetVSync(bool enabled) override;
		
		inline void SetPosition(int windowX, int windowY) override
		{
			m_WindowData.WindowX = windowX;
			m_WindowData.WindowY = windowY;
			glfwSetWindowPos(m_WindowHandle, windowX, windowY);
		}

		inline void Maximize() override
		{
			glfwMaximizeWindow(m_WindowHandle);
		}

		inline void Restore() override
		{
			glfwRestoreWindow(m_WindowHandle);
		}

		inline const WindowData& GetData() const override
		{
			return m_WindowData;
		}
		
		inline void* GetNativeHandle() const
		{
			return m_WindowHandle;
		}

		std::vector<VideoMode> GetMonitorVideoModes() const override;

	private:
		GLFWwindow* m_WindowHandle;
		WindowData  m_WindowData;
		std::unique_ptr<GraphicsContext> m_GraphicsContext;
	};
}