#include "pch.h"

#include "DesktopWindow.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"

namespace Vision
{
	static unsigned int s_WindowCount = 0;
	
	static void GLFWErrorCallback(int error, const char* description)
	{
		VN_CORE_ERROR("GLFW Error {0} : {1}", error, description);
	}

	DesktopWindow::DesktopWindow(const WindowData& data)
	{
		m_WindowData.Title     = data.Title;
		m_WindowData.Width     = data.Width;
		m_WindowData.Height    = data.Height;
		m_WindowData.WindowX   = data.WindowX;
		m_WindowData.WindowY   = data.WindowY;
		m_WindowData.VSync     = data.VSync;
		m_WindowData.Maximized = data.Maximized;
		m_WindowData.Mode      = data.Mode;
		
		if (s_WindowCount == 0)
		{
			int sucess = glfwInit();
			VN_CORE_ASSERT(sucess, "Unable To Intialize GLFW");

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef VN_DIST
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		const char* title = data.Title.c_str();

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

		int montiorX, montiorY, monitorWidth, monitorHeight;
		glfwGetMonitorWorkarea(monitor, &montiorX, &montiorY, &monitorWidth, &monitorHeight);
		
		switch (data.Mode)
		{
			case WindowMode::Windowed:
			{
				m_WindowHandle = 
					glfwCreateWindow((int)data.Width, (int)data.Height, title, nullptr, nullptr);
				
				int32 centerX = montiorX + (monitorWidth - data.Width) / 2;
				int32 centerY = montiorY + (monitorHeight - data.Height) / 2;

				SetPosition(centerX, centerY);
			}
			break;

			case WindowMode::Fullscreen:
			{
				m_WindowHandle = glfwCreateWindow((int)data.Width, (int)data.Height, title, monitor, nullptr);
			}
			break;
		}
		
		++s_WindowCount;

		m_GraphicsContext = GraphicsContext::Create(this);

		glfwSetWindowUserPointer(m_WindowHandle, &m_WindowData);

		glfwSetWindowCloseCallback(m_WindowHandle, [](GLFWwindow* window) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetWindowSizeCallback(m_WindowHandle, [](GLFWwindow* window, int width, int height) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = width;
			data.Height = height;

			WindowResizeEvent event(width, height);
			data.EventCallback(event);
		});

		glfwSetWindowFocusCallback(m_WindowHandle, [](GLFWwindow* window, int focused) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (focused)
			{
				WindowFocusEvent event;
				data.EventCallback(event);
			}
			else
			{
				WindowLostFocusEvent event;
				data.EventCallback(event);
			}
		});

		glfwSetWindowPosCallback(m_WindowHandle, [](GLFWwindow* window, int32 windowX, int32 windowY) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.WindowX = windowX;
			data.WindowY = windowY;

			WindowMovedEvent event(windowX, windowY);
			data.EventCallback(event);
		});

		glfwSetWindowIconifyCallback(m_WindowHandle, [](GLFWwindow* window, int32 iconified)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			
			if (iconified)
			{
				WindowMinimizedEvent event;
				data.EventCallback(event);
			}
			else
			{
				WindowRestoredEvent event;
				data.EventCallback(event);
			}
		});

		glfwSetWindowMaximizeCallback(m_WindowHandle, [](GLFWwindow* window, int32 maximized)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);
			data.Maximized = maximized;

			if (maximized)
			{
				WindowMaximizedEvent event;
				data.EventCallback(event);
			}
			else
			{
				WindowRestoredEvent event;
				data.EventCallback(event);
			}
		});
			
		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
				}
				break;

				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
				}
				break;

				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
				}
				break;
			}
		});

		glfwSetCharCallback(m_WindowHandle, [](GLFWwindow* window, uint32 codepoint) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event((unsigned char)codepoint);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int32 button, int32 action, int32 mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					data.EventCallback(event);
				}
				break;
				
				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					data.EventCallback(event);
				}
				break;
			}
		});

		glfwSetCursorPosCallback(m_WindowHandle, [](GLFWwindow* window, double xpos, double ypos) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event(static_cast<float>(xpos), static_cast<float>(ypos));
			data.EventCallback(event);
		});

		glfwSetScrollCallback(m_WindowHandle, [](GLFWwindow* window, double xoffset, double yoffset) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseWheelScrolledEvent event(static_cast<float>(xoffset), static_cast<float>(yoffset));
			data.EventCallback(event);
		});

		glfwSetCursorEnterCallback(m_WindowHandle, [](GLFWwindow* window, int entered)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			if (entered)
			{
				MouseCursorEnteredEvent event;
				data.EventCallback(event);
			}
			else
			{
				MouseCursorLeftEvent event;
				data.EventCallback(event);
			}
		});
	}

	DesktopWindow::~DesktopWindow()
	{
		glfwDestroyWindow(m_WindowHandle);
		--s_WindowCount;

		if (s_WindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void DesktopWindow::OnUpdate() 
	{
		glfwPollEvents();
		m_GraphicsContext->SwapBuffers();
	}

	void DesktopWindow::SetMode(WindowMode mode)
	{
		if (m_WindowData.Mode == mode)
		{
			const char* modeName = (mode == WindowMode::Windowed) ? "Windowed" : "Fullscreen";
			VN_CORE_INFO("WindowMode Is Already Set To {0}", modeName);
			return;
		}

		m_WindowData.Mode = mode;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		
		switch (mode)
		{
			case WindowMode::Windowed:
			{
				glfwSetWindowMonitor(m_WindowHandle, nullptr, 0, 0, m_WindowData.Width, m_WindowData.Height, videoMode->refreshRate);
				
				int montiorX, montiorY, monitorWidth, monitorHeight;
				glfwGetMonitorWorkarea(monitor, &montiorX, &montiorY, &monitorWidth, &monitorHeight);
				
				int centerX = montiorX + (monitorWidth - m_WindowData.Width) / 2;
				int centerY = montiorY + (monitorHeight - m_WindowData.Height) / 2;

				SetPosition(centerX, centerY);
			}
			break;
		
			case WindowMode::Fullscreen:
			{
				glfwSetWindowMonitor(m_WindowHandle, monitor, 0, 0, m_WindowData.Width, m_WindowData.Height, videoMode->refreshRate);
			}
			break;
		}
	}

	void DesktopWindow::SetVSync(bool enabled)
	{
		if (enabled == m_WindowData.VSync)
		{
			return;
		}

		m_GraphicsContext->SetVSync(enabled);
		m_WindowData.VSync = enabled;
	}

	std::vector<VideoMode> DesktopWindow::GetMonitorVideoModes() const
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		
		int count;
		const GLFWvidmode* modes = glfwGetVideoModes(monitor, &count);

		std::vector<VideoMode> videoModes;
		videoModes.reserve(count);

		for (int i = 0; i < count; i++)
		{
			VideoMode currentVideoMode;
			
			currentVideoMode.Width = modes[i].width;
			currentVideoMode.Height = modes[i].height;
			currentVideoMode.RefreshRate = modes[i].refreshRate;

			videoModes.emplace_back(currentVideoMode);
		}

		std::reverse(videoModes.begin(), videoModes.end());

		return videoModes;
	}
}