#include "pch.h"

#include "Vision/Platform/Window.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	static uint32 InternalWindowCount = 0;

	static void GLFWErrorCallback(int32 error,
								  const char* description)
	{
		VnCoreError("GLFW Error {0} : {1}", error, description);
	}

	void OpenWindow(Window* window,
					const std::string& title,
					uint32 width,
					uint32 height,
					EventCallbackFn eventCallback,
					WindowMode mode,
					int32 windowX,
					int32 windowY)
	{
		window->Title = title;

		window->Width  = width;
		window->Height = height;

		window->WindowX = windowX;
		window->WindowY = windowY;

		window->Mode = mode;

		window->EventCallback = eventCallback;

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

		switch (mode)
		{
			case WindowMode::Windowed:
			{
				window->Handle = glfwCreateWindow((int32)width,
												  (int32)height,
												  title.c_str(),
												  nullptr,
												  nullptr);

				int32 centerX = montiorX + (monitorWidth - width) / 2;
				int32 centerY = montiorY + (monitorHeight - height) / 2;

				SetWindowPosition(window, centerX, centerY);
			}
			break;

			case WindowMode::Fullscreen:
			{
				window->Handle = glfwCreateWindow((int32)width,
												  (int32)height,
												  title.c_str(),
												  primaryMonitor,
												  nullptr);
			}
			break;
		}

		++InternalWindowCount;

		glfwSetWindowUserPointer((GLFWwindow*)(window->Handle), window);

		glfwSetWindowCloseCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);
			WindowCloseEvent event;
			userWindow.EventCallback(event);
		});

		glfwSetWindowSizeCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 width, int32 height)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			userWindow.Width  = width;
			userWindow.Height = height;

			WindowResizeEvent event(width, height);
			userWindow.EventCallback(event);
		});

		glfwSetWindowFocusCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, int32 focused)
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

		glfwSetWindowPosCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 windowX, int32 windowY)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			userWindow.WindowX = windowX;
			userWindow.WindowY = windowY;

			WindowMovedEvent event(windowX, windowY);
			userWindow.EventCallback(event);
		});

		glfwSetWindowIconifyCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 iconified)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);
			userWindow.Maximized = !iconified;

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

		glfwSetWindowMaximizeCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, int32 maximized)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);
			userWindow.Maximized = maximized;

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

		glfwSetKeyCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
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

		glfwSetCharCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, uint32 codepoint)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event((unsigned char)codepoint);
			userWindow.EventCallback(event);
		});

		glfwSetMouseButtonCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, int32 button, int32 action, int32 mods)
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

		glfwSetCursorPosCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, double xpos, double ypos)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event(static_cast<float32>(xpos), static_cast<float32>(ypos));
			userWindow.EventCallback(event);
		});

		glfwSetScrollCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, double xoffset, double yoffset)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			MouseWheelScrolledEvent event(static_cast<float32>(xoffset), static_cast<float32>(yoffset));
			userWindow.EventCallback(event);
		});

		glfwSetCursorEnterCallback((GLFWwindow*)window->Handle, [](GLFWwindow* window, int entered)
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

	void CloseWindow(Window* window)
	{
		glfwDestroyWindow((GLFWwindow*)window->Handle);

		--InternalWindowCount;

		if (InternalWindowCount == 0)
		{
			glfwTerminate();
		}
	}

	void PollEvents()
	{
		glfwPollEvents();
	}

	void SetWindowTitle(Window* window,
						const std::string& title)
	{
		glfwSetWindowTitle((GLFWwindow*)window->Handle,
						   title.c_str());
	}

	void SetWindowSize(Window* window,
					   uint32 width,
					   uint32 height)
	{
		VnCoreAssert(width > 0 && height > 0, "invalid window size");
		glfwSetWindowSize((GLFWwindow*)window->Handle,
						  (int32)width,
						  (int32)height);
	}

	void SetWindowPosition(Window* window,
						   int32 windowX,
						   int32 windowY)
	{
		window->WindowX = windowX;
		window->WindowY = windowY;
		glfwSetWindowPos((GLFWwindow*)window->Handle,
						 windowX,
						 windowY);
	}

	void MaximizeWindow(Window* window)
	{
		glfwMaximizeWindow((GLFWwindow*)window->Handle);
	}

	void MinimizeWindow(Window* window)
	{
		glfwIconifyWindow((GLFWwindow*)window->Handle);
	}

	void SetWindowMode(Window* window, WindowMode mode)
	{
		if (window->Mode == mode)
		{
			VnCoreInfo("window mode is already set to {0}", (mode == WindowMode::Windowed) ? "Windowed" : "Fullscreen");
			return;
		}

		window->Mode = mode;

		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

		switch (mode)
		{
			case WindowMode::Windowed:
			{
				glfwSetWindowMonitor((GLFWwindow*)window->Handle,
									 nullptr,
									 0,
									 0,
									 window->Width,
									 window->Height,
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

				int32 centerX = montiorX + (monitorWidth - window->Width) / 2;
				int32 centerY = montiorY + (monitorHeight - window->Height) / 2;

				SetWindowPosition(window, centerX, centerY);
			}
			break;

			case WindowMode::Fullscreen:
			{
				glfwSetWindowMonitor((GLFWwindow*)window->Handle,
									 monitor,
									 0,
									 0,
									 window->Width,
									 window->Height,
									 videoMode->refreshRate);

			}
			break;
		}
	}
}