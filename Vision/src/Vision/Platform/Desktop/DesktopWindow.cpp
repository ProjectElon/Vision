#include "pch.h"

#include "Vision/Platform/Window.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"
#include "Vision/Core/Application.h"
#include "Vision/Core/Vars.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	static uint32 InternalWindowCount = 0;

	static void GLFWErrorCallback(int32 error,
								  const char* description)
	{
		VnCoreError("GLFW Error {0} : {1}", error, description);
	}

	void Window::Open(const char* title,
					  uint32 width,
					  uint32 height,
					  EventCallbackFn eventCallback,
					  bool fullScreen,
					  int32 windowX,
					  int32 windowY)
	{
		Vision::Settings& settings = Application::Get().Vars.Settings;

		settings.WindowTitle  = title;
		settings.WindowWidth  = width;
		settings.WindowHeight = height;
		settings.WindowX 	  = windowX;
		settings.WindowY 	  = windowY;
		settings.FullScreen   = fullScreen;

		EventCallback = eventCallback;
		Settings 	  = &settings;

		if (InternalWindowCount == 0)
		{
			int32 sucess = glfwInit();
			VnCoreAssert(sucess, "unable to intialize GLFW");

			glfwSetErrorCallback(GLFWErrorCallback);
		}

		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 5);
		glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifndef VN_DIST
		glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

		/*
		@(Harlequin): Right now we are handling one monitor.
		*/
		GLFWmonitor* primaryMonitor = glfwGetPrimaryMonitor();

		int32 montiorX;
		int32 montiorY;
		int32 monitorWidth;
		int32 monitorHeight;

		glfwGetMonitorWorkarea(primaryMonitor,
							   &montiorX,
							   &montiorY,
							   &monitorWidth,
							   &monitorHeight);

		if (fullScreen)
		{
			Handle = glfwCreateWindow(static_cast<int32>(width),
											  static_cast<int32>(height),
											  title,
											  primaryMonitor,
											  nullptr);
		}
		else
		{
			Handle = glfwCreateWindow(static_cast<int32>(width),
											  static_cast<int32>(height),
											  title,
											  nullptr,
											  nullptr);

			int32 centerX = montiorX + (monitorWidth - width) / 2;
			int32 centerY = montiorY + (monitorHeight - height) / 2;

			SetPosition(centerX, centerY);
		}

		++InternalWindowCount;

		glfwSetWindowUserPointer((GLFWwindow*)(Handle), this);

		glfwSetWindowCloseCallback((GLFWwindow*)(Handle), [](GLFWwindow* window)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			userWindow.EventCallback(event);
		});

		glfwSetWindowSizeCallback((GLFWwindow*)(Handle), [](GLFWwindow* window, int32 width, int32 height)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);
			userWindow.Settings->WindowWidth = width;
			userWindow.Settings->WindowHeight = height;

			WindowResizeEvent event(width, height);
			userWindow.EventCallback(event);
		});

		glfwSetWindowFocusCallback((GLFWwindow*)Handle, [](GLFWwindow* window, int32 focused)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			if (focused)
			{
				WindowFocusEvent event;
				userWindow.EventCallback(event);
			}
			else
			{
				WindowLostFocusEvent event;
				userWindow.EventCallback(event);
			}
		});

		glfwSetWindowPosCallback((GLFWwindow*)(Handle), [](GLFWwindow* window, int32 windowX, int32 windowY)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			userWindow.Settings->WindowX = windowX;
			userWindow.Settings->WindowY = windowY;

			WindowMovedEvent event(windowX, windowY);
			userWindow.EventCallback(event);
		});

		glfwSetWindowIconifyCallback((GLFWwindow*)(Handle), [](GLFWwindow* window, int32 iconified)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);
			userWindow.Settings->IsWindowMaximized = !iconified;

			if (iconified)
			{
				WindowMinimizedEvent event;
				userWindow.EventCallback(event);
			}
			else
			{
				WindowRestoredEvent event;
				userWindow.EventCallback(event);
			}
		});

		glfwSetWindowMaximizeCallback((GLFWwindow*)Handle, [](GLFWwindow* window, int32 maximized)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);
			userWindow.Settings->IsWindowMaximized = maximized;

			if (maximized)
			{
				WindowMaximizedEvent event;
				userWindow.EventCallback(event);
			}
			else
			{
				WindowRestoredEvent event;
				userWindow.EventCallback(event);
			}
		});

		glfwSetKeyCallback((GLFWwindow*)Handle, [](GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
		{
			Window& userWindow = *(Window*)(glfwGetWindowUserPointer(window));

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					userWindow.EventCallback(event);
				}
				break;

				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					userWindow.EventCallback(event);
				}
				break;

				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					userWindow.EventCallback(event);
				}
				break;
			}
		});

		glfwSetCharCallback((GLFWwindow*)Handle, [](GLFWwindow* window, uint32 codepoint)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event((unsigned char)codepoint);
			userWindow.EventCallback(event);
		});

		glfwSetMouseButtonCallback((GLFWwindow*)Handle, [](GLFWwindow* window, int32 button, int32 action, int32 mods)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					userWindow.EventCallback(event);
				}
				break;

				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					userWindow.EventCallback(event);
				}
				break;
			}
		});

		glfwSetCursorPosCallback((GLFWwindow*)Handle, [](GLFWwindow* window, double xpos, double ypos)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event(static_cast<float32>(xpos), static_cast<float32>(ypos));
			userWindow.EventCallback(event);
		});

		glfwSetScrollCallback((GLFWwindow*)Handle, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			MouseWheelScrolledEvent event(static_cast<float32>(xoffset), static_cast<float32>(yoffset));
			userWindow.EventCallback(event);
		});

		glfwSetCursorEnterCallback((GLFWwindow*)Handle, [](GLFWwindow* window, int entered)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			if (entered)
			{
				MouseCursorEnteredEvent event;
				userWindow.EventCallback(event);
			}
			else
			{
				MouseCursorLeftEvent event;
				userWindow.EventCallback(event);
			}
		});
	}

	void Window::Close()
	{
		glfwDestroyWindow((GLFWwindow*)Handle);

		--InternalWindowCount;

		if (InternalWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void Window::PollEvents()
	{
		glfwPollEvents();
	}

	void Window::SetTitle(const char* title)
	{
		glfwSetWindowTitle((GLFWwindow*)Handle, title);
		Settings->WindowTitle = title;
	}

	void Window::SetSize(uint32 width,
					     uint32 height)
	{
		VnCoreAssert(width != 0 && height != 0, "invalid window size");

		Settings->WindowWidth = width;
		Settings->WindowHeight = height;

		glfwSetWindowSize((GLFWwindow*)Handle,
						  (int32)width,
						  (int32)height);
	}

	void Window::SetPosition(int32 windowX,
						     int32 windowY)
	{
		Settings->WindowX = windowX;
		Settings->WindowY = windowY;

		glfwSetWindowPos((GLFWwindow*)Handle,
						 windowX,
						 windowY);
	}

	void Window::Maximize()
	{
		glfwMaximizeWindow((GLFWwindow*)Handle);
	}

	void Window::Minimize()
	{
		glfwIconifyWindow((GLFWwindow*)Handle);
	}

	void Window::SetMode(bool fullScreen)
	{
		if (Settings->FullScreen == fullScreen)
		{
			VnCoreInfo("window mode is already set to {0}", (fullScreen) ? "fullscreen" : "windowed");
			return;
		}

		Settings->FullScreen = fullScreen;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

		if (fullScreen)
		{
			glfwSetWindowMonitor((GLFWwindow*)Handle,
								 monitor,
								 0,
								 0,
								 Settings->WindowWidth,
								 Settings->WindowHeight,
								 videoMode->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor((GLFWwindow*)Handle,
								 nullptr,
								 0,
								 0,
								 Settings->WindowWidth,
								 Settings->WindowHeight,
								 videoMode->refreshRate);

			int32 montiorX;
			int32 montiorY;
			int32 monitorWidth;
			int32 monitorHeight;

			glfwGetMonitorWorkarea(monitor,
								   &montiorX,
								   &montiorY,
								   &monitorWidth,
								   &monitorHeight);

			int32 centerX = montiorX + (monitorWidth - Settings->WindowWidth) / 2;
			int32 centerY = montiorY + (monitorHeight - Settings->WindowHeight) / 2;

			SetPosition(centerX, centerY);
		}
	}
}