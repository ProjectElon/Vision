#include "Menubar.h"

#include "Dialog.h"
#include "EditorLayer.h"

#include "Vision/Entity/Scene.h"

#include <imgui.h>

namespace Vision
{
	void Menubar::SetEditorLayer(EditorLayer* editorLayer)
	{
		m_EditorLayer = editorLayer;
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
				m_EditorLayer->OpenScene();
			}

			Scene* scene = Scene::GetActiveScene();
			bool enabled = scene != nullptr;

			if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, enabled))
			{
				m_EditorLayer->SaveScene(scene);
			}

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S", false, enabled))
			{
				m_EditorLayer->SaveSceneAs(scene);
			}

			if (ImGui::MenuItem("Close Scene", "Ctrl+E", false, enabled))
			{
				m_EditorLayer->CloseScene(scene);
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}
}