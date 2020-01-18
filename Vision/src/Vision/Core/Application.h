#pragma once

#include "Vision/Events/Event.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/KeyEvent.h"
#include "Vision/Events/MouseEvent.h"
#include "Log.h"

namespace Vision
{
	class Application
	{
	public:
		Application();
		virtual ~Application() {}

		void Run();

		bool OnWindowResize(WindowResizeEvent& e);

	private:
		bool m_Running = true;
	};

	/* To be defined in CLIENT */
	Application* CreateApplication();
}