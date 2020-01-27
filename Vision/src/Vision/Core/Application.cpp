#include "pch.h"
#include "Application.h"
#include "Vision/Platform/EntryPoint.h"
#include "Vision/Core/Core.h"
#include "Vision/Core/Log.h"
#include "Vision/Platform/Input.h"
#include "Vision/Renderer/GraphicsContext.h"

namespace Vision
{
	Application* Application::s_Instance = nullptr;

	Application::Application()
	{
		VN_CORE_ASSERT(!s_Instance, "Application already exists");
		s_Instance = this;

		GraphicsContext::SetRenderApi(RenderApi::OpenGL);

		m_Window = Window::Create();
		m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
		m_Window->SetVSync(true);
		
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

	void Application::Run()
	{
		while (m_Running)
		{
			m_FrameTimer->Stop();
			float deltaTime = (float)m_FrameTimer->GetElapsedTime();
			m_FrameTimer->Start();

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

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));
		dispatcher.Dispatch<WindowMinimizedEvent>(BIND_EVENT_FN(Application::OnWindowMinimized));
		dispatcher.Dispatch<WindowRestoredEvent>(BIND_EVENT_FN(Application::OnWindowRestored));
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

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
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
}