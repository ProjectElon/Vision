#include "pch.h"
#include "Application.h"
#include "Vision/Core/Common.h"
#include "Vision/Core/Log.h"
#include "Vision/Platform/Input.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/IO/Assets.h"

namespace Vision
{
	Application::Application()
	{
		VnCoreAssert(!Instance, "Application already exists");

		Instance = this;

		Window.Open(Vars.Settings.WindowTitle.Text,
					Vars.Settings.WindowWidth,
					Vars.Settings.WindowHeight,
					VnBindEventFn(Application::OnEvent));

		Renderer::Init(&Window);
		Renderer2D::Init();
		Assets::Init();
		Input::Init();

		PushOverlay(&ImGuiLayer);

		FrameTimer.Start();

		Renderer::SetViewport(0,
							  0,
							  Vars.Settings.WindowWidth,
							  Vars.Settings.WindowHeight);
	}

	Application::~Application()
	{
		for (auto* layer : LayerStack)
		{
			layer->OnDetach();
		}

		Input::Shutdown();
		Assets::Shutdown();
		Renderer2D::Shutdown();
		Renderer::Shutdown();
	}

	void Application::Run()
	{
		while (Running)
		{
			FrameTimer.Stop();
			float deltaTime = static_cast<float>(FrameTimer.ElapsedTime);
			FrameTimer.Start();

			if (!Minimized)
			{
 				for (Layer* layer : LayerStack)
				{
					layer->OnUpdate(deltaTime);
				}

				ImGuiLayer.Begin();

				for (Layer* layer : LayerStack)
				{
					layer->OnImGuiRender();
				}

				ImGuiLayer.End();
			}

			Window.PollEvents();

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

		for (auto layerIt = LayerStack.rbegin(); layerIt != LayerStack.rend(); ++layerIt)
		{
			(*layerIt)->OnEvent(e);

			if (e.Handled)
			{
				break;
			}
		}
	}

	void Application::PushLayer(Layer* layer)
	{
		LayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Layer* overlay)
	{
		LayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	void Application::Close()
	{
		Running = false;
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
		Minimized = true;
		return false;
	}

	bool Application::OnWindowRestored(WindowRestoredEvent& e)
	{
		Minimized = false;
		return false;
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		Running = false;
		return true;
	}

	Application* Application::Instance = nullptr;
}