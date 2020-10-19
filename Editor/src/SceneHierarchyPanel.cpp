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

		uint32 flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;
		
		ImGui::TextColored({ 0.931f, 0.618f, 0.265f, 1.0f }, "Max Entity Count: %d", scene.GetMaxEntityCount());

		std::string sceneDisplaytext = scene.Name + "(" + std::to_string(scene.GetEntityCount()) + ")";

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
					
				if (expanded)
				{ 
					ImGui::TreePop();
				}
			});

			ImGui::TreePop();
		}

		if (ImGui::Button("Create Entity"))
		{
			ImGui::OpenPopup("Create Entity Popup");
		}

		static char buffer[1024];

		if (ImGui::BeginPopup("Create Entity Popup"))
		{
			memset(buffer, 0, 1024);

			if (ImGui::InputText("Tag", buffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue) ||
				ImGui::IsItemDeactivated())
			{
				std::string tag = buffer;

				if (!tag.empty())
				{
					Entity entity = scene.CreateEntity(tag);

					if (entity)
					{
						if (scene.EditorState.SeleteEntityTag.empty())
						{
							scene.EditorState.SeleteEntityTag = tag;
						}

						ImGui::CloseCurrentPopup();
					}
				}
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}