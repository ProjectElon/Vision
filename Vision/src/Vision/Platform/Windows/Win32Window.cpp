#include "pch.hpp"

#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"

#include "Vision/Platform/Window.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"
#include "Vision/Core/Application.h"
#include "Vision/Core/Vars.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	using EventCallbackFn = std::function<void(Event&)>;

	void OpenWindow(Window* window,
	                const char* title,
	                int32 xPosition,
	                int32 yPosition,
		  			uint32 width,
		  			uint32 height,
  		  			bool fullScreen)
	{
		Vision::Settings& settings = Application::Get().Vars.Settings;

		int32 sucess = glfwInit();
		VnCoreAssert(sucess, "failed to intialize GLFW");

		glfwWindowHint(GLFW_DOUBLEBUFFER, GLFW_TRUE);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
		glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
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
			window->Handle = glfwCreateWindow(static_cast<int32>(width),
											  static_cast<int32>(height),
											  title,
											  primaryMonitor,
											  nullptr);
		}
		else
		{
			window->Handle = glfwCreateWindow(static_cast<int32>(width),
											  static_cast<int32>(height),
											  title,
											  nullptr,
											  nullptr);

			int32 centerX = montiorX + (monitorWidth - width) / 2;
			int32 centerY = montiorY + (monitorHeight - height) / 2;

			SetWindowPosition(window, centerX, centerY);
		}

		glfwMaximizeWindow(static_cast<GLFWwindow*>(window->Handle));

		glfwSetWindowCloseCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window)
		{
			WindowCloseEvent event;
			Application::Get().OnEvent(event);
		});

		glfwSetWindowSizeCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 width, int32 height)
		{
			WindowResizeEvent event(width, height);

			Application& app = Application::Get();
			app.OnEvent(event);
			app.Vars.Settings.WindowWidth  = width;
			app.Vars.Settings.WindowHeight = height;
		});

		glfwSetWindowFocusCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 focused)
		{
			if (focused)
			{
				WindowFocusEvent event;
				Application::Get().OnEvent(event);
			}
			else
			{
				WindowLostFocusEvent event;
				Application::Get().OnEvent(event);
			}
		});

		glfwSetWindowPosCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 windowX, int32 windowY)
		{
			WindowMovedEvent event(windowX, windowY);
			Application::Get().OnEvent(event);
		});

		glfwSetWindowIconifyCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 iconified)
		{
			if (iconified)
			{
				WindowMinimizedEvent event;
				Application::Get().OnEvent(event);
			}
			else
			{
				WindowRestoredEvent event;
				Application::Get().OnEvent(event);
			}
		});

		glfwSetWindowMaximizeCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 maximized)
		{
			if (maximized)
			{
				WindowMaximizedEvent event;
				Application::Get().OnEvent(event);
			}
			else
			{
				WindowRestoredEvent event;
				Application::Get().OnEvent(event);
			}
		});

		glfwSetKeyCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 key, int32 scancode, int32 action, int32 mods)
		{
			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					Application::Get().OnEvent(event);
				}
				break;

				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					Application::Get().OnEvent(event);
				}
				break;

				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					Application::Get().OnEvent(event);
				}
				break;
			}
		});

		glfwSetCharCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, uint32 codepoint)
		{
			KeyTypedEvent event((unsigned char)codepoint);
			Application::Get().OnEvent(event);
		});

		glfwSetMouseButtonCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int32 button, int32 action, int32 mods)
		{
			Window& userWindow = *(Window*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					MouseButtonPressedEvent event(button);
					Application::Get().OnEvent(event);
				}
				break;

				case GLFW_RELEASE:
				{
					MouseButtonReleasedEvent event(button);
					Application::Get().OnEvent(event);
				}
				break;
			}
		});

		glfwSetCursorPosCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, double xpos, double ypos)
		{
			MouseMovedEvent event(static_cast<float32>(xpos), static_cast<float32>(ypos));
			Application::Get().OnEvent(event);
		});

		glfwSetScrollCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, double xoffset, double yoffset)
		{
			MouseWheelScrolledEvent event(static_cast<float32>(xoffset), static_cast<float32>(yoffset));
			Application::Get().OnEvent(event);
		});

		glfwSetCursorEnterCallback((GLFWwindow*)(window->Handle), [](GLFWwindow* window, int entered)
		{
			if (entered)
			{
				MouseCursorEnteredEvent event;
				Application::Get().OnEvent(event);
			}
			else
			{
				MouseCursorLeftEvent event;
				Application::Get().OnEvent(event);
			}
		});
	}

	void CloseWindow(Window *window)
	{
		glfwDestroyWindow(static_cast<GLFWwindow*>(window->Handle));
		glfwTerminate();
	}

	void PumpEvents(Window *window)
	{
		glfwPollEvents();
	}

	void SetWindowTitle(Window *window, const char* title)
	{
		glfwSetWindowTitle(static_cast<GLFWwindow*>(window->Handle), title);
	}

	void SetWindowSize(Window *window,
					   uint32 width,
					   uint32 height)
	{
		glfwSetWindowSize(static_cast<GLFWwindow*>(window->Handle),
						  (int32)width,
						  (int32)height);
	}

	void SetWindowPosition(Window *window,
						   int32 windowX,
						   int32 windowY)
	{
		glfwSetWindowPos(static_cast<GLFWwindow*>(window->Handle),
						 windowX,
						 windowY);
	}

	void SetWindowMode(Window *window, bool fullScreen)
	{
		GLFWmonitor* monitor = glfwGetPrimaryMonitor();
		const GLFWvidmode* videoMode = glfwGetVideoMode(monitor);

		int32 windowWidth, windowHeight;
		glfwGetWindowSize(static_cast<GLFWwindow*>(window->Handle), &windowWidth, &windowHeight);

		if (fullScreen)
		{
			glfwSetWindowMonitor(static_cast<GLFWwindow*>(window->Handle),
								 monitor,
								 0,
								 0,
								 windowWidth,
								 windowHeight,
								 videoMode->refreshRate);
		}
		else
		{
			glfwSetWindowMonitor(static_cast<GLFWwindow*>(window->Handle),
								 nullptr,
								 0,
								 0,
								 windowWidth,
								 windowHeight,
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

			int32 centerX = montiorX + (monitorWidth - windowWidth) / 2;
			int32 centerY = montiorY + (monitorHeight - windowHeight) / 2;

			SetWindowPosition(window, centerX, centerY);
		}
	}
}