#pragma once

#include "Vision/Events/Event.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"
#include "Vision/Core/LayerStack.h"
#include "Vision/Platform/Window.h"
#include "Vision/ImGui/ImGuiLayer.h"
#include "Vision/Platform/Timer.h"

namespace Vision
{
	class Application
	{
	public:
		Application();
		virtual ~Application();
	
		void Run();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		inline Window& GetWindow() const { return (*m_Window); }

		inline static Application& Get() { return *s_Instance; }
		
	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowMinimized(WindowMinimizedEvent& e);
		bool OnWindowRestored(WindowRestoredEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		ImGuiLayer* m_ImGuiLayer;

		LayerStack m_LayerStack;
		std::unique_ptr<Timer> m_FrameTimer;

		bool m_Running = true;
		bool m_Minimized = false;
		bool m_VSync = false;

	private:
		static Application* s_Instance;
	};

	/* To be defined in CLIENT */
	Application* CreateApplication();
}