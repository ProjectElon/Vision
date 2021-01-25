#include "pch.h"
#include "Application.h"
#include "Vision/Core/Base.h"
#include "Vision/Core/Log.h"
#include "Vision/Platform/Input.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/IO/Assets.h"

namespace Vision
{
	Application::Application()
	{
		VnCoreAssert(!s_Instance, "Application already exists");

		s_Instance = this;

		OpenWindow(&m_Window,
				   "Vision",
					1280,
					720,
					VnBindEventFn(Application::OnEvent));

		Renderer::Init(&m_Window);
		Renderer2D::Init();
		Assets::Init();
		Input::Init();

		m_ImGuiLayer = new ImGuiLayer();
		PushOverlay(m_ImGuiLayer);

		m_FrameTimer.Start();

		Renderer::SetViewport(0, 0, m_Window.Width, m_Window.Height);
	}

	Application::~Application()
	{
		Input::Shutdown();
		Assets::Shutdown();
		Renderer2D::Shutdown();
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (m_Running)
		{
			m_FrameTimer.Stop();
			float deltaTime = (float)m_FrameTimer.ElapsedTime;
			m_FrameTimer.Start();

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

			PollEvents();
			Renderer::SwapBuffers();

#ifdef VN_PLATFORM_WINDOWS
	#ifdef VN_EDITOR
				// @(Note): win32 file watchers
				MsgWaitForMultipleObjectsEx(0,
											NULL,
											0,
											QS_ALLINPUT,
											MWMO_ALERTABLE);
	#endif
#endif
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);

		dispatcher.Dispatch<WindowResizeEvent>(VnBindEventFn(Application::OnWindowResize));
		dispatcher.Dispatch<WindowMinimizedEvent>(VnBindEventFn(Application::OnWindowMinimized));
		dispatcher.Dispatch<WindowRestoredEvent>(VnBindEventFn(Application::OnWindowRestored));
		dispatcher.Dispatch<WindowCloseEvent>(VnBindEventFn(Application::OnWindowClose));

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

	void Application::Close()
	{
		m_Running = false;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		Renderer::SetViewport(0,
							  0,
							  e.GetWidth(),
							  e.GetHeight());
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