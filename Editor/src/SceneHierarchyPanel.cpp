#include <Vision.h>

#include "SceneHierarchyPanel.h"

namespace Vision
{
	void SceneHierarchyPanel::OnImGuiRender()
	{
		Scene& scene = Scene::GetActiveScene();
		
		ImGui::Begin("Scene Hierarchy");

		if (ImGui::TreeNodeEx(scene.Name.c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
		{
			scene.EachEntity([&](EntityHandle entity)
			{
				auto& tag = scene.GetComponent<TagComponent>(entity).Tag;
				const char* entityName = tag.c_str();

				ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding;

				if (entity == scene.SelectedEntity)
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool expanded = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, entityName);
				bool isSeleted = ImGui::IsItemClicked() || ImGui::IsItemToggledOpen();
				
				if (isSeleted && scene.SelectedEntity != entity)
				{
					VN_CORE_INFO("Entity: {0} Selected", tag);
					scene.SelectedEntity = entity;
				}
					
				if (expanded) ImGui::TreePop();
			});

			ImGui::TreePop();
		}

		ImGui::End();
	}
}