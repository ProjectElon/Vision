#include <Vision.h>

#include "SceneHierarchyPanel.h"
#include <imgui.h>

namespace Vision
{
	void SceneHierarchyPanel::OnImGuiRender()
	{
		Scene& scene = Scene::GetActiveScene();
		auto& editorState = Scene::EditorState;

		ImGui::Begin("Scene Hierarchy");

		uint32 flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;
		
		if (ImGui::TreeNodeEx(scene.Name.c_str(), flags))
		{
			scene.EachEntity([&](EntityHandle entity)
			{
				auto& tag = scene.GetComponent<TagComponent>(entity).Tag;
				const char* entityName = tag.c_str();

				flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;

				if (entity == editorState.SelectedEntity)
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool expanded  = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, entityName);
				bool isSeleted = ImGui::IsItemClicked() || ImGui::IsItemToggledOpen();
				
				if (isSeleted && editorState.SelectedEntity != entity)
				{
					VN_CORE_INFO("Entity: {0} Selected", tag);
					editorState.SelectedEntity = entity;
				}
					
				if (expanded)
				{ 
					ImGui::TreePop();
				}
			});

			ImGui::TreePop();
		}

		ImGui::End();
	}
}