#include "Menubar.h"

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