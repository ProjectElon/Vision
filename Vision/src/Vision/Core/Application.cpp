#include "pch.h"
#include "Application.h"
#include "Vision/Core/Core.h"
#include "Vision/Core/Log.h"
#include "Vision/Platform/Input.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/RenderCommand.h"

namespace Vision
{
	Application::Application()
	{
		VN_CORE_ASSERT(!s_Instance, "Application already exists");
		
		s_Instance = this;

		Renderer::SetAPI(Renderer::API::OpenGL);

		m_Window = Window::Create();
		m_Window->SetEventCallback(VN_BIND_EVENT_FN(Application::OnEvent));
		
		Renderer::Init();
		Input::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_FrameTimer = Timer::Create();
		m_FrameTimer->Start();

		Renderer::OnWindowResize(m_Window->GetWidth(), m_Window->GetHeight());
	}

	Application::~Application()
	{
		Input::ShutDown();
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_FrameTimer->Stop();
			float deltaTime = (float)m_FrameTimer->GetElapsedTime();
			m_FrameTimer->Start();

			if (!m_Minimized)
			{
				m_ImGuiLayer->Begin();

				for (Layer* layer : m_LayerStack)
				{
					layer->OnImGuiRender();
				}

				m_ImGuiLayer->End();

 				for (Layer* layer : m_LayerStack)
				{
					layer->OnUpdate(deltaTime);
				}
			}

			m_Window->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(VN_BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowMinimizedEvent>(VN_BIND_EVENT_FN(Application::OnWindowMinimized));
		dispatcher.Dispatch<WindowRestoredEvent>(VN_BIND_EVENT_FN(Application::OnWindowRestored));
		dispatcher.Dispatch<WindowCloseEvent>(VN_BIND_EVENT_FN(Application::OnWindowClose));

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

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Renderer::OnWindowResize(e.GetWidth(), e.GetHeight());
		return false;
	}

	bool Application::OnWindowMinimized(WindowMinimizedEvent& e)
	{
		m_Minimized = true;
		return false;
	}

	bool Application::OnWindowRestored(WindowRestoredEvent& e)
	{
		m_Minimized = false;
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		m_Running = false;
		return true;
	}

	Application* Application::s_Instance = nullptr;
}