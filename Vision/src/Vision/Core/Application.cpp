#include "pch.h"
#include "Application.h"
#include "Vision/Platform/EntryPoint.h"
#include "Vision/Core/Core.h"
#include "Vision/Core/Log.h"
#include "Vision/Platform/Input.h"

#include <glad/glad.h>

namespace Vision
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		VN_CORE_ASSERT(!s_Instance, "Application Already Exists");
		s_Instance = this;

		m_Window = Window::Create();
		m_Window->SetVSync(true);
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Input::Initialize();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_FrameTimer = Timer::Create();
		m_FrameTimer->Start();
	}

	Application::~Application()
	{
		Input::ShutDown();
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it)
		{
			if ((*it)->IsEnabled())
			{
				(*it)->OnEvent(e);

				if (e.Handled)
				{
					break;
				}
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		m_LayerStack.PushLayer(layer);
		
		layer->OnAttach();

		if (layer->IsEnabled())
		{
			layer->Enable();
		}
	}
	
	void Application::PushOverlay(Layer* overlay)
	{
		m_LayerStack.PushOverlay(overlay);
		
		overlay->OnAttach();

		if (overlay->IsEnabled())
		{
			overlay->Enable();
		}
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_FrameTimer->Stop();
			float deltaTime = (float)m_FrameTimer->GetElapsedTime();
			VN_CORE_INFO("Delta Time : {0}", deltaTime);
			m_FrameTimer->Start();

			glClearColor(1, 0, 1, 1);
			glClear(GL_COLOR_BUFFER_BIT);

			if (!m_Minimized)
			{
				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(deltaTime);
				}

				m_ImGuiLayer->Begin();
				
				for (Layer* layer : m_LayerStack)
				{
					layer->OnImGuiRender();
				}

				m_ImGuiLayer->End();
			}

			m_Window->OnUpdate();
		}
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		if (e.GetWidth() == 0 || e.GetHeight() == 0)
		{
			m_Minimized = true;
		}
		else
		{
			m_Minimized = false;
		}

		glViewport(0, 0, e.GetWidth(), e.GetHeight());

		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}
}