#include "Menubar.h"

#include "Dialog.h"
#include "EditorLayer.h"

#include "Vision/Entity/Scene.h"

#include <imgui.h>

namespace Vision
{
	void Menubar::SetEditor(EditorLayer* editor)
	{
		m_Editor = editor;
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
				m_Editor->OpenScene();
			}

			bool enabled = m_Editor->m_ActiveScene != 0;

			if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, enabled))
			{
				m_Editor->SaveActiveScene();
			}

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S", false, enabled))
			{
				m_Editor->SaveActiveSceneAs();
			}

			if (ImGui::MenuItem("Close Scene", "Ctrl+E", false, enabled))
			{
				m_Editor->CloseActiveScene();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}