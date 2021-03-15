#include "SceneHierarchyPanel.h"
#include "Vision/IO/FileSystem.h"


#include <imgui.h>

namespace Vision
{
	void SceneHierarchyPanel::OnImGuiRender()
	{
		EditorState& editorState = Scene::EditorState;

		ImGui::Begin("Scene Hierarchy");

		IsInteractable = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

		if (ActiveSceneID)
		{
			Scene* scene = Assets::GetScene(ActiveSceneID);

			// Deselect Entity
			{
				if (ImGui::IsMouseDown(0) &&
					ImGui::IsWindowHovered() &&
					!editorState.SelectedEntityTag.empty())
				{
					editorState.SelectedEntityTag = "";
				}
			}

			// Create Empty Entity when right clicking on blank space
			{
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
			}

			// Drawing Entites
			{
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
		}

		ImGui::End();
	}

	void SceneHierarchyPanel::DrawEntity(Scene* scene, Entity entity)
	{
		EditorState& editorState = Scene::EditorState;

		auto& tagComponent = scene->GetComponent<TagComponent>(entity);

		const std::string& tag = tagComponent.Tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

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