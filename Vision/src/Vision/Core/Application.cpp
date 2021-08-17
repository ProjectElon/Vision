#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Application.h"
#include "Vision/Core/Logger.h"
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
		
		const char *windowTitle = Vars.Settings.WindowTitle.Data;
		
		OpenWindow(
			&Window,
			windowTitle,
			0,
			0,
			Vars.Settings.WindowWidth,
			Vars.Settings.WindowHeight);

		Logging::Initialize();
		Input::Initialize();
		Assets::Initialize();
		Renderer::Initialize();
		Renderer2D::Initialize();

		PushOverlay(&ImGuiLayer);

		ViewportRect viewportRect = { 0, 0, Vars.Settings.WindowWidth, Vars.Settings.WindowHeight };
		Renderer::API.SetViewport(&viewportRect);
	}

	Application::~Application()
	{
		for (auto* layer : LayerStack.Layers)
		{
			layer->OnDetach();
		}

		Renderer2D::Shutdown();
		Renderer::Shutdown();
		Assets::Shutdown();
		Input::Shutdown();
		Logging::Shutdown();
	}

	void Application::Run()
	{
		FrameTimer.Start();

		while (Running)
		{
			FrameTimer.Stop();
			auto deltaTime = static_cast<float32>(FrameTimer.ElapsedTime);
			FrameTimer.Start();

			PumpEvents(&Window);

			if (!Minimized)
			{
 				for (Layer* layer : LayerStack.Layers)
				{
					layer->OnUpdate(deltaTime);
				}

				ImGuiLayer.Begin();

				for (Layer* layer : LayerStack.Layers)
				{
					layer->OnImGuiRender();
				}

				ImGuiLayer.End();
			}

			Renderer::API.SwapBuffers();

#ifdef VN_PLATFORM_WINDOWS
	#ifdef VN_EDITOR
				// @(temparary): for win32 file watchers
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

		for (auto layerIt = LayerStack.Layers.rbegin();
			 layerIt != LayerStack.Layers.rend();
			 ++layerIt)
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
#ifndef VN_EDITOR
		ViewportRect viewportRect = { 0, 0, e.GetWidth(), e.GetHeight() };
		Renderer::API.SetViewport(&viewportRect);
#endif
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