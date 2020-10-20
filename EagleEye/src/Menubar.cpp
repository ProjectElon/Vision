#include "Menubar.h"
#include <imgui.h>

namespace Vision
{
	void Menubar::OnImGuiRender()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File", true))
		{
			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}	
}