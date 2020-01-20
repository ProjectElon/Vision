#pragma once

#include "Vision/Platform/Window.h"

#include <GLFW/glfw3.h>

namespace Vision
{
	struct WindowData
	{
		std::string Title;
		unsigned int Width;
		unsigned int Height;
		bool VSync;

		EventCallbackFn EventCallback;
	};

	class DesktopWindow : public Window
	{
	public:
		DesktopWindow(const WindowProps& props);
		virtual ~DesktopWindow();

		void OnUpdate() override;

		inline const std::string& GetTitle() override { return m_Data.Title; }
		inline unsigned int GetWidth()  override { return m_Data.Width; }
		inline unsigned int GetHeight() override { return m_Data.Height; }

		inline void SetEventCallback(const EventCallbackFn& fn) override { m_Data.EventCallback = fn; }
		void SetVSync(bool enabled) override;
		bool IsVSyncEnabled() const override;

		inline virtual void* GetNativeWindow() const override { return m_Window; }

	private:
		GLFWwindow* m_Window;
		WindowData m_Data;
	};
}