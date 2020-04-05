#pragma once

#include "Vision/Core/Layer.h"
#include "Vision/Platform/Window.h"

namespace Vision
{
	class ImGuiLayer : public Layer
	{
	public:
		ImGuiLayer();
		~ImGuiLayer() = default;

		void OnAttach() override;
		void OnDetach() override;
		void OnImGuiRender() override;

		void Begin();
		void End();

	private:
		Window* m_Window;
		
		std::vector<VideoMode> m_VideoModes;
		std::vector<std::string> m_VideoModesString;
		std::vector<const char*> m_VideoModesFormat;

		int m_CurrentWindowModeSelected;
		int m_PreviousWindowModeSelected;

		int m_CurrentWindowSizeSelected;
		int m_PreviousWindowSizeSelected;

		bool m_PreviousVSyncSelected = false;
		bool m_VSyncSelected = false;
	};
}