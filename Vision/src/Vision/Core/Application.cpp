#include "pch.h"
#include "Application.h"

namespace Vision
{
	Application::Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(800, 600);

		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		
		while (m_Running)
		{
		}
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		VN_CORE_TRACE(__FUNCTION__);
		VN_CORE_INFO(e);
		return true;
	}
}