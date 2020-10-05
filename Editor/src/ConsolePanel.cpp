#include "ConsolePanel.h"
#include <imgui.h>

namespace Vision
{
	void ConsolePanel::OnImGuiRender()
	{
		ImGui::Begin("Console");
		ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
		ImGui::End();
	}
}