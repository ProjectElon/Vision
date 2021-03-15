#pragma once

#include "Vision/Events/Event.h"
#include "Vision/Core/Vars.h"
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
		Vision::Window Window;
		Vision::Vars Vars;

		Vision::ImGuiLayer ImGuiLayer;

		Vision::LayerStack LayerStack;
		Timer FrameTimer;

		bool Running   = true;
		bool Minimized = false;

		Application();
		virtual ~Application();

		void Run();

		void OnEvent(Event& e);
		void PushLayer(Layer* layer);
		void PushOverlay(Layer* overlay);

		void Close();

		inline static Application& Get()
		{
			return *Instance;
		}

	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowMinimized(WindowMinimizedEvent& e);
		bool OnWindowRestored(WindowRestoredEvent& e);

	private:
		static Application* Instance;
	};

	/* To be defined in Client */
	Application* CreateApplication();
}