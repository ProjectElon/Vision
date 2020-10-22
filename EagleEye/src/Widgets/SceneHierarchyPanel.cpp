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

		// Deselect Entity
		{
			if (ImGui::IsMouseDown(0) &&
				ImGui::IsWindowHovered() &&
			    !scene.EditorState.SeleteEntityTag.empty())
			{
				scene.EditorState.SeleteEntityTag = "";
			}
		}

		// Create Empty Entity when right clicking on blank space
		{
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
		}

		/*
		// Max Entites Constraint
		{
			ImGui::Text("Max Entites");
			ImGui::SameLine();

			int32 maxEntityCount = scene.MaxEntityCount;
			int32 entityCount = scene.EntityCount;

			if (ImGui::DragInt("##Max Entites", &maxEntityCount, 1.0f, entityCount, MAXINT))
			{
				scene.MaxEntityCount = maxEntityCount;
			}
		}
		*/

		// Drawing Entites
		{
			std::string sceneDisplaytext = scene.Name + " (" + std::to_string(scene.EntityCount) + "/" + std::to_string(scene.MaxEntityCount) + ")";

			uint32 flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

			if (ImGui::TreeNodeEx(sceneDisplaytext.c_str(), flags))
			{
				scene.EachEntity([&](Entity entity)
				{
					DrawEntity(entity);
				});

				ImGui::TreePop();
			}

			ImGui::End();
		}
	}

	void SceneHierarchyPanel::DrawEntity(Entity entity)
	{
		Scene& scene = Scene::GetActiveScene();
		EditorState& editorState = Scene::EditorState;

		auto& tagComponent = scene.GetComponent<TagComponent>(entity);

		const std::string& tag = tagComponent.Tag;

		ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_SpanAvailWidth | ImGuiTreeNodeFlags_FramePadding;

		if (tag == editorState.SeleteEntityTag)
		{
			flags |= ImGuiTreeNodeFlags_Selected;
		}

		bool open     = ImGui::TreeNodeEx((void*)(intptr_t)entity, flags, tag.c_str());
		bool selected = ImGui::IsItemClicked() || ImGui::IsItemToggledOpen();

		if (selected && tag != editorState.SeleteEntityTag)
		{
			editorState.SeleteEntityTag = tag;
		}

		if (ImGui::BeginPopupContextItem())
		{
			if (ImGui::MenuItem("Delete Entity"))
			{
				if (tag == editorState.SeleteEntityTag && !editorState.SeleteEntityTag.empty())
				{
					editorState.SeleteEntityTag = "";
				}

				scene.FreeEntity(tag);
			}

			ImGui::EndPopup();
		}

		if (open)
		{
			ImGui::TreePop();
		}
	}
}