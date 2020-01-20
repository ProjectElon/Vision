#pragma once

#include "Vision/Events/Event.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"
#include "Vision/Core/LayerStack.h"
#include "Vision/Platform/Window.h"

namespace Vision
{
	class Application
	{
	public:
		Application();
		virtual ~Application() {}
	
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void OnEvent(Event& e);
		void Run();

		inline static Application& Get() { return *s_Instance; }

	private:
		
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);

	private:
		std::unique_ptr<Window> m_Window;
		LayerStack m_LayerStack;

		bool m_Running = true;
		bool m_Minimized = false;
		
		static Application* s_Instance;
	};

	/* To be defined in CLIENT */
	Application* CreateApplication();
}