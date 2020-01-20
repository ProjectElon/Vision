#include "pch.h"
#include "Application.h"

#include "Vision/Core/Core.h"
#include "Vision/Core/Log.h"

namespace Vision
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		VN_CORE_ASSERT(!s_Instance, "Application Already Exists");
		s_Instance = this;

		m_Window = Window::Create();
		VN_CORE_INFO("{0} Window Created ({1}, {2})", m_Window->GetTitle(), m_Window->GetWidth(), m_Window->GetHeight());
		
		m_Window->SetVSync(true);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
	}

	void Application::OnEvent(Event& e)
	{
		VN_CORE_INFO(e);

		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			(*it)->OnEvent(e);
			
			if (e.Handled)
			{
				break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		layer->OnAttach();
	}
	
	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
			return false;
		}

		m_Minimized = false;

		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}