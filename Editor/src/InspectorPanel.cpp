#include "InspectorPanel.h"
#include "Vision/Entity/EditorState.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Vision
{

    static void DrawVector3UIControl(const std::string& label,
        glm::vec3& values,
        float columnWidth = 100.0f,
        float resetValue = 0.0f)
    {
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(3, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

        if (ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##X", &values.x, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

        if (ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Y", &values.y, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

        if (ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
        }

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        ImGui::DragFloat("##Z", &values.z, 0.1f);
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();
    }

	InspectorPanel::InspectorPanel()
	{
        auto& editorState = Scene::EditorState;

		InspectComponent<TagComponent, false>("Tag", [&](void* component)
		{
            char buffer[1024];

            Scene& scene = Scene::GetActiveScene();
			auto& tagComponent = ComponentCast<TagComponent>(component);
            std::string& oldTag = tagComponent.Tag;

            memset(buffer, 0, 1024);
			memcpy(buffer, tagComponent.Tag.data(), tagComponent.Tag.size());

			if (ImGui::InputText("##Tag", buffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue) ||
				ImGui::IsItemDeactivated())
			{
                std::string newTag = buffer;

                if (scene.QueryEntity(newTag) == entity::null)
				{
                    Entity entity = scene.QueryEntity(editorState.SeleteEntityTag);

                    scene.m_Tags.emplace(newTag, entity);
                    scene.m_Tags.erase(oldTag);

                    if (oldTag == editorState.SeleteEntityTag)
                    {
                        editorState.SeleteEntityTag = newTag;
                    }

                    if (oldTag == scene.ActiveCameraTag)
                    {
                        scene.ActiveCameraTag = newTag;
                    }

                    oldTag = newTag;
				}
                else
                {
                    VN_CORE_INFO("Tag is Already Taken");
                }
			}
		});

        InspectComponent<TransformComponent>("Transform", [&](void* component)
        {
            auto& transform = ComponentCast<TransformComponent>(component);

            DrawVector3UIControl("Position", transform.Position);

            glm::vec3 rotation = glm::degrees(transform.Rotation);

            DrawVector3UIControl("Rotation", rotation);

            transform.Rotation = glm::radians(rotation);

            DrawVector3UIControl("Scale", transform.Scale, 100.0f, 1.0f);
        });

        InspectComponent<OrthographicCameraComponent>("Orthographic Camera", [&](void* component)
        {
            Scene& scene = Scene::GetActiveScene();

            auto& camera = ComponentCast<OrthographicCameraComponent>(component);

            static bool primary = false;

            primary = editorState.SeleteEntityTag == scene.ActiveCameraTag;

            if (ImGui::Checkbox("Primary", &primary))
            {
                scene.ActiveCameraTag = editorState.SeleteEntityTag;
            }

            ImGui::Checkbox("Static", &camera.Static);

            if (camera.Static)
            {
                ImGui::TextDisabled("Aspect Ratio: %f", camera.AspectRatio);
                ImGui::TextDisabled("Size: %f", camera.Size);
                ImGui::TextDisabled("Near: %f", camera.Near);
                ImGui::TextDisabled("Far: %f", camera.Far);
            }
            else
            {
                bool changed = false;

                changed |= ImGui::InputFloat("Aspect Ratio", &camera.AspectRatio);
                changed |= ImGui::InputFloat("Size", &camera.Size);
                changed |= ImGui::InputFloat("Near", &camera.Near);
                changed |= ImGui::InputFloat("Far", &camera.Far);

                if (changed)
                {
                    camera.Projection = glm::ortho(-camera.AspectRatio * camera.Size,
                                                    camera.AspectRatio * camera.Size,
                                                   -camera.Size,
                                                    camera.Size,
                                                    camera.Near,
                                                    camera.Far);
                }
            }
        });

        InspectComponent<SpriteRendererComponent>("Sprite Renderer", [&](void* component)
        {
            auto& sprite = ComponentCast<SpriteRendererComponent>(component);

            const TextureData& textureData = sprite.Texture->GetData();

            std::string textureName = "Texture: " + textureData.Name + "\n";
            ImGui::Text(textureName.c_str());

            ImGui::Image((void*)(intptr_t)textureData.RendererID, ImVec2(64, 64));

            ImGui::Text("\n");

            // Texture Properties
            {
                ImGui::Text("Texture Properties");

                const TextureProps& properties = sprite.Texture->GetProperties();

                static const char* WrapModeStrings[] =
                {
                    "Repeat",
                    "Clamp To Edge"
                };

                static const char* FilterModeStrings[] =
                {
                    "Point",
                    "Bilinear"
                };

                int32 seletedWrapXMode  = (int32)properties.WrapX;
                int32 seletedWrapYMode  = (int32)properties.WrapY;
                int32 seletedFilterMode = (int32)properties.FilterMode;

                if (ImGui::Combo("Wrap X", &seletedWrapXMode, WrapModeStrings, 2))
                {
                    if (seletedWrapXMode != (int32)properties.WrapX)
                    {
                        sprite.Texture->SetWrapMode((WrapMode)seletedWrapXMode, (WrapMode)seletedWrapYMode);
                    }
                }

                if (ImGui::Combo("Wrap Y", &seletedWrapYMode, WrapModeStrings, 2))
                {
                    if (seletedWrapYMode != (int32)properties.WrapY)
                    {
                        sprite.Texture->SetWrapMode((WrapMode)seletedWrapXMode, (WrapMode)seletedWrapYMode);
                    }
                }

                if (ImGui::Combo("Filter Mode", &seletedFilterMode, FilterModeStrings, 2))
                {
                    if (seletedFilterMode != (int32)properties.FilterMode)
                    {
                        sprite.Texture->SetFilterMode((FilterMode)seletedFilterMode);
                    }
                }
            }

            ImGui::Text("\n");

            ImGui::ColorEdit4("Color", &sprite.Color.r);

            ImGui::Text("\n");

            ImGui::InputFloat2("Bottom Left Point", &sprite.BottomLeftPoint.x);
            ImGui::InputFloat2("Top Right Point", &sprite.TopRightPoint.x);

            ImGui::Text("\n");
            ImGui::Checkbox("Flip X", &sprite.FlipX);
            ImGui::Checkbox("Flip Y", &sprite.FlipY);
        });
	}

	void InspectorPanel::OnImGuiRender()
	{
		auto& scene = Scene::GetActiveScene();
        auto& editorState = Scene::EditorState;
        auto& componentInspector = editorState.ComponentInspector;

		ImGui::Begin("Inspector");

        Entity seletedEntity = scene.QueryEntity(editorState.SeleteEntityTag);

		// @Note: SeletedEntity can be invalid if we deleted the entity...
		if (seletedEntity)
		{
			const auto& storage = scene.m_Entites[seletedEntity];

            auto entityStorageIter = storage.begin();

            while (entityStorageIter != storage.end())
            {
                ComponentID componentID = entityStorageIter->first;
                ComponentIndex componentIndex = entityStorageIter->second;

                entityStorageIter++;

                const ComponentInfo& componentInfo = componentInspector[componentID];
                ComponentState& componentState = m_ComponentState[{ seletedEntity, componentID }];
                const char* componentName = componentInfo.Name.c_str();

                uint32 flags = ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding;

                ImGui::SetNextTreeNodeOpen(componentState.Expanded);
                componentState.Expanded = ImGui::TreeNodeEx((void*)(intptr_t)componentID, flags, componentName);

                if (componentInfo.Removable)
                {
                    ImGui::SameLine(0, 0);

                    ImGui::Text("  ");

                    ImGui::SameLine(0, 0);

                    if (ImGui::Button("-", { 18, 18 }))
                    {
                        VN_CORE_INFO("\tRemoving Component: {0}", componentInfo.Name);
                        m_ComponentState[{ seletedEntity, componentID }] = ComponentState();
                        scene.RemoveComponent(seletedEntity, componentID);
                    }
                }

                const auto& inspectFn = componentInfo.InspectFn;

                if (componentState.Expanded)
                {
                    if (inspectFn)
                    {
                        void* componentMemory = scene.GetComponentMemory(componentID, componentIndex);
                        inspectFn(componentMemory);
                    }
                    else
                    {
                        ImGui::Text("ComponentID: %lld", componentID);
                        ImGui::Text("ComponentIndex: %d", componentIndex);
                    }

                    ImGui::TreePop();
                }

                ImGui::Text("\n");
            }

			if (ImGui::Button("Add Component"))
			{
                if (storage.size() < componentInspector.size())
                {
                    ImGui::OpenPopup("Add Component Popup");
                }
			}

			if (ImGui::BeginPopup("Add Component Popup"))
			{
				for (const auto& [componentID, componentInfo] : componentInspector)
				{
					if (storage.find(componentID) != storage.end())
					{
						continue;
					}

                    const char* componentName = componentInfo.Name.c_str();

					if (ImGui::Selectable(componentName))
					{
                        componentInfo.AddFn(seletedEntity);
						VN_CORE_INFO("\tAdded Component: {0}", componentName);
					}
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}