#include <Vision.h>

#include "SceneHierarchyPanel.h"
#include <imgui.h>

namespace Vision
{
	void SceneHierarchyPanel::OnImGuiRender()
	{
		Scene& scene = Scene::GetActiveScene();
		EditorState& editorState = Scene::EditorState;

		ImGui::Begin("Scene Hierarchy");

		m_IsInteractable = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

		if (ImGui::BeginPopupContextWindow(0, 1, false))
		{
			if (ImGui::MenuItem("Create Empty Entity"))
			{
				Entity entity = scene.CreateEntity("Empty Entity");

				if (entity)
				{
					if (scene.EditorState.SeleteEntityTag.empty())
					{
						scene.EditorState.SeleteEntityTag = "Empty Entity";
					}
				}
			}

			ImGui::EndPopup();
		}

		ImGui::Text("Max Entites");
		ImGui::SameLine();

		int32 maxEntityCount = scene.MaxEntityCount;
		int32 entityCount = scene.EntityCount;
		
		if (ImGui::DragInt("##Max Entites", &maxEntityCount, 1.0f, entityCount, MAXINT))
		{
			scene.MaxEntityCount = maxEntityCount;
			// @Note: We need to reallocate Entites and Components memory or simply Reload Scene
		}

		std::string sceneDisplaytext = scene.Name + " (" + std::to_string(scene.EntityCount) + "/" + std::to_string(scene.MaxEntityCount) + ")";

		uint32 flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;
		
		if (ImGui::TreeNodeEx(sceneDisplaytext.c_str(), flags))
		{
			scene.EachEntity([&](Entity entity)
			{
				auto& tagComponent = scene.GetComponent<TagComponent>(entity);
				const std::string& tag = tagComponent.Tag;

				flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;

				if (tag == editorState.SeleteEntityTag)
				{
					flags |= ImGuiTreeNodeFlags_Selected;
				}

				bool expanded  = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, tag.c_str());
				bool isSeleted = ImGui::IsItemClicked() || ImGui::IsItemToggledOpen();

				if (isSeleted && editorState.SeleteEntityTag != tag)
				{
					VN_CORE_INFO("Entity: {0} Selected", tag);
					editorState.SeleteEntityTag = tag;
				}

				if (ImGui::BeginPopupContextItem())
				{
					if (ImGui::MenuItem("Delete Entity"))
					{
						if (tag == editorState.SeleteEntityTag)
						{
							editorState.SeleteEntityTag = "";
						}

						scene.FreeEntity(tag);
					}

					ImGui::EndPopup();
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