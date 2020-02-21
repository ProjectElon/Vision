#include "pch.h"
#include "ImGuiLayer.h"
#include "Vision/Core/Application.h"

#include <imgui.h>
#include <examples/imgui_impl_glfw.h>
#include <examples/imgui_impl_opengl3.h>

#include <GLFW/glfw3.h>
#include <glad/glad.h>

#if VN_PLATFORM_DESKTOP

	#define IMGUI_IMPL_OPENGL_LOADER_GLAD
	#include <examples/imgui_impl_opengl3.cpp>
	#include <examples/imgui_impl_glfw.cpp>
	
#endif

namespace Vision
{
	ImGuiLayer::ImGuiLayer()
		: Layer("ImGui")
	{
		Application& app = Application::Get();
		m_Window = &app.GetWindow();
		m_Window->SetVSync(m_VSyncSelected);

		if (m_Window->GetMode() == WindowMode::Windowed)
		{
			m_CurrentWindowModeSelected = m_PreviousWindowModeSelected = 0;
		}
		else
		{
			m_CurrentWindowModeSelected = m_PreviousWindowModeSelected = 1;
		}

		m_VideoModes = m_Window->GetMonitorVideoModes();
		m_VideoModesString.reserve(m_VideoModes.size());
		m_VideoModesFormat.reserve(m_VideoModes.size());

		for (uint32_t i = 0; i < m_VideoModes.size(); ++i)
		{
			const VideoMode& videoMode = m_VideoModes[i];

			if (videoMode.Width == m_Window->GetWidth() && videoMode.Height == m_Window->GetHeight())
			{
				m_CurrentWindowSizeSelected = m_PreviousWindowSizeSelected = i;
			}

			std::stringstream ss;
			ss << videoMode.Width << "x" << videoMode.Height << " " << videoMode.RefreshRate << " hz";

			m_VideoModesString.emplace_back(ss.str());
			m_VideoModesFormat.emplace_back(m_VideoModesString.back().c_str());
		}
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& io = ImGui::GetIO(); (void)io;
		io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		ImGui::StyleColorsDark();

		ImGuiStyle& style = ImGui::GetStyle();
		
		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		GLFWwindow* window = static_cast<GLFWwindow*>(m_Window->GetNativeWindowHandle());

		ImGui_ImplGlfw_InitForOpenGL(window, true);
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{
		ImGui_ImplOpenGL3_Shutdown();
		ImGui_ImplGlfw_Shutdown();
		ImGui::DestroyContext();
	}

	void ImGuiLayer::Begin()
	{
		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();
	}

	void ImGuiLayer::End() 
	{
		ImGuiIO& io = ImGui::GetIO();

		Application& app = Application::Get();
		io.DisplaySize = ImVec2((float)app.GetWindow().GetWidth(), (float)app.GetWindow().GetHeight());

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			GLFWwindow* backupCurrentContext = glfwGetCurrentContext();
			ImGui::UpdatePlatformWindows();
			ImGui::RenderPlatformWindowsDefault();
			glfwMakeContextCurrent(backupCurrentContext);
		}
	}

	void ImGuiLayer::OnImGuiRender()
	{
		ImGui::Begin("Engine Configurations");
		
		const char* windowModesItems[] = { "Windowed", "Fullscreen" };
		ImGui::Combo("Window Mode", &m_CurrentWindowModeSelected, windowModesItems, 2);

		if (m_CurrentWindowModeSelected != m_PreviousWindowModeSelected)
		{
			m_PreviousWindowModeSelected = m_CurrentWindowModeSelected;
			
			if (m_CurrentWindowModeSelected == 0)
			{
				m_Window->SetMode(WindowMode::Windowed);
			}
			else if (m_CurrentWindowModeSelected == 1)
			{
				m_Window->SetMode(WindowMode::Fullscreen);
			}
		}

		ImGui::Combo("Select Resulotion", &m_CurrentWindowSizeSelected, m_VideoModesFormat.data(), m_VideoModes.size());

		if (m_CurrentWindowSizeSelected != m_PreviousWindowSizeSelected)
		{
			m_PreviousWindowSizeSelected = m_CurrentWindowSizeSelected;
			const VideoMode& mode = m_VideoModes[m_CurrentWindowSizeSelected];
			m_Window->SetSize(mode.Width, mode.Height);
		}

		ImGui::Checkbox("VSync", &m_VSyncSelected);

		if (m_PreviousVSyncSelected != m_VSyncSelected)
		{
			m_PreviousVSyncSelected = m_VSyncSelected;
			m_Window->SetVSync(m_VSyncSelected);
		}

		ImGui::End();
	}
}