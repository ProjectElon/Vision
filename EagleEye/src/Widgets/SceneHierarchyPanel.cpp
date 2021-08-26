#include "SceneHierarchyPanel.h"
#include "Vision/Platform/FileSystem.h"
#include "EagleEye.h"

#include <imgui.h>

namespace Vision
{
	void SceneHierarchyPanel::OnImGuiRender()
	{
		EditorState& editorState = EagleEye::EditorState;

		ImGui::Begin("Scene Hierarchy");

		IsInteractable = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

		if (ActiveSceneID)
		{
			Scene* scene = Assets::GetScene(ActiveSceneID);

			if (ImGui::IsMouseDown(0) &&
				ImGui::IsWindowHovered() &&
				!editorState.SelectedEntityTag.empty())
			{
				editorState.SelectedEntityTag = "";
			}

			if (ImGui::BeginPopupContextWindow(0, 1, false))
			{
				if (ImGui::MenuItem("Create Empty Entity"))
				{
					Entity entity = scene->CreateEntity("Empty Entity");

					if (entity)
					{
						if (editorState.SelectedEntityTag.empty())
						{
							editorState.SelectedEntityTag = "Empty Entity";
						}
					}
				}

				ImGui::EndPopup();
			}

			const Asset& sceneAsset = Assets::GetAsset(ActiveSceneID);
			std::string sceneName = FileSystem::GetFileName(sceneAsset.Path, false);
			std::string sceneText = sceneName + " (" + std::to_string(scene->EntityCount) + "/" + std::to_string(scene->MaxEntityCount) + ")";

			uint32 flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

			if (ImGui::TreeNodeEx(sceneText.c_str(), flags))
			{
				scene->EachEntity([&](Entity entity)
				{
					DrawEntity(scene, entity);
				});

				ImGui::TreePop();
			}

		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntity(Scene* scene, Entity entity)
	{
		EditorState& editorState = EagleEye::EditorState;

		auto& tagComponent = scene->GetComponent<TagComponent>(entity);

		const std::string& tag = tagComponent.Tag.Data;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_FramePadding;

		if (tag == editorState.SelectedEntityTag)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool open     = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, tag.c_str());
		bool selected = ImGui::IsItemClicked() || ImGui::IsItemToggledOpen();

		if (selected && tag != editorState.SelectedEntityTag)
		{
			editorState.SelectedEntityTag = tag;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				if (tag == editorState.SelectedEntityTag && !editorState.SelectedEntityTag.empty())
				{
					editorState.SelectedEntityTag = "";
				}

				scene->FreeEntity(tag);
			}

			ImGui::EndPopup();
		}

		if (open)
		{
			ImGui::TreePop();
		}
	}
}