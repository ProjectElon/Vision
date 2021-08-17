#include "Menubar.h"

#include "Dialog.h"
#include "EditorLayer.h"

#include "Vision/Entity/Scene.h"

#include <imgui.h>

namespace Vision
{
	Menubar::Menubar(EditorLayer* editor)
		: Editor(editor)
	{
	}

	void Menubar::OnImGuiRender()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File", true))
		{
			if (ImGui::MenuItem("New Scene...", "Ctrl+Shift+N"))
			{
				Dialog::Open(DialogType::CreateScene);
			}

			if (ImGui::MenuItem("Open Scene...", "Ctrl+Shift+O"))
			{
				Editor->OpenSceneDialog();
			}

			bool enabled = Editor->ActiveSceneID != 0;

			if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, enabled))
			{
				Editor->SaveActiveScene();
			}

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S", false, enabled))
			{
				Editor->SaveActiveSceneAs();
			}

			if (ImGui::MenuItem("Close Scene", "Ctrl+E", false, enabled))
			{
				Editor->CloseActiveScene();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}