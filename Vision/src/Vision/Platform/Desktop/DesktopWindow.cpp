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

	DesktopWindow::DesktopWindow(const WindowProps& props)
	{
		m_Data.Title  = props.Title;
		m_Data.Width  = props.Width;
		m_Data.Height = props.Height;
		m_Data.VSync  = props.VSync;
		m_Data.Mode   = props.Mode;
		
		if (s_WindowCount == 0)
		{
			int sucess = glfwInit();
			VN_CORE_ASSERT(sucess, "Unable To Intialize GLFW");

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_RED_BITS,   (int)props.RedBits);
		glfwWindowHint(GLFW_GREEN_BITS, (int)props.GreenBits);
		glfwWindowHint(GLFW_BLUE_BITS,  (int)props.BlueBits);
		glfwWindowHint(GLFW_ALPHA_BITS, (int)props.AlphaBits);

		glfwWindowHint(GLFW_DEPTH_BITS,   (int)props.DepthBits);
		glfwWindowHint(GLFW_STENCIL_BITS, (int)props.StencilBits);
		glfwWindowHint(GLFW_SAMPLES,      (int)props.Samples);
		glfwWindowHint(GLFW_DOUBLEBUFFER, (int)props.BufferCount == 2);
		
		glfwWindowHint(GLFW_AUTO_ICONIFY, GLFW_FALSE);

#ifndef VN_DIST
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		const char* title = props.Title.c_str();

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

		int montiorX, montiorY, monitorWidth, monitorHeight;
		glfwGetMonitorWorkarea(monitor, &montiorX, &montiorY, &monitorWidth, &monitorHeight);
		
		switch (props.Mode)
		{
			case WindowMode::Windowed:
			{
				m_WindowHandle = 
					glfwCreateWindow((int)props.Width, (int)props.Height, title, nullptr, nullptr);
				
				int centerX = montiorX + (monitorWidth - props.Width) / 2;
				int centerY = montiorY + (monitorHeight - props.Height) / 2;

				SetPosition(centerX, centerY);
			}
			break;

			case WindowMode::Fullscreen:
			{
				m_WindowHandle = glfwCreateWindow((int)props.Width, (int)props.Height, title, monitor, nullptr);
			}
			break;
		}
		
		++s_WindowCount;

		m_GraphicsContext = GraphicsContext::Create(this);

		glfwSetWindowUserPointer(m_WindowHandle, &m_Data);

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

		glfwSetWindowPosCallback(m_WindowHandle, [](GLFWwindow* window, int xpos, int ypos) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.XPosition = xpos;
			data.YPosition = ypos;

			WindowMovedEvent event(xpos, ypos);
			data.EventCallback(event);
		});

		glfwSetWindowIconifyCallback(m_WindowHandle, [](GLFWwindow* window, int iconified)
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

		glfwSetWindowMaximizeCallback(m_WindowHandle, [](GLFWwindow* window, int maximized)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

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
			
		glfwSetKeyCallback(m_WindowHandle, [](GLFWwindow* window, int key, int scancode, int action, int mods)
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

		glfwSetCharCallback(m_WindowHandle, [](GLFWwindow* window, unsigned int codepoint) 
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event((unsigned char)codepoint);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(m_WindowHandle, [](GLFWwindow* window, int button, int action, int mods)
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
		if (m_Data.Mode == mode)
		{
			const char* modeName = (mode == WindowMode::Windowed) ? "Windowed" : "Fullscreen";
			VN_CORE_INFO("WindowMode Is Already Set To {0}", modeName);
			return;
		}

		m_Data.Mode = mode;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);
		
		switch (mode)
		{
			case WindowMode::Windowed:
			{
				glfwSetWindowMonitor(m_WindowHandle, nullptr, 0, 0, m_Data.Width, m_Data.Height, videoMode->refreshRate);
				
				int montiorX, montiorY, monitorWidth, monitorHeight;
				glfwGetMonitorWorkarea(monitor, &montiorX, &montiorY, &monitorWidth, &monitorHeight);
				
				int centerX = montiorX + (monitorWidth - m_Data.Width) / 2;
				int centerY = montiorY + (monitorHeight - m_Data.Height) / 2;

				SetPosition(centerX, centerY);
			}
			break;
		
			case WindowMode::Fullscreen:
			{
				glfwSetWindowMonitor(m_WindowHandle, monitor, 0, 0, m_Data.Width, m_Data.Height, videoMode->refreshRate);
			}
			break;
		}
	}

	void DesktopWindow::SetVSync(bool enabled)
	{
		if (enabled == m_Data.VSync)
		{
			VN_CORE_INFO("VSync is already {0}", (enabled) ? "On" : "Off");
			return;
		}

		m_GraphicsContext->SetVSync(enabled);
		m_Data.VSync = enabled;
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