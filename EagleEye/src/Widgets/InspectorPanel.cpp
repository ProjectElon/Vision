#include "InspectorPanel.h"
#include "Vision/Entity/EditorState.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Vision
{
	InspectorPanel::InspectorPanel()
	{
        EditorState& editorState = Scene::EditorState;

        // adding tag manually

        ComponentInfo& info = editorState.ComponentMeta[typeid(TagComponent).hash_code()];
        info.Name = "Tag";
        info.Removable = false;

        InspectComponent<TransformComponent>("Transform", [&](void* component)
        {
            auto& transform = ComponentCast<TransformComponent>(component);

            ImGuiWidgets::DrawFloat3("Position", transform.Position, 78.0f);

            glm::vec3 rotation = glm::degrees(transform.Rotation);

            ImGuiWidgets::DrawFloat3("Rotation", rotation, 78.0f);

            transform.Rotation = glm::radians(rotation);

            ImGuiWidgets::DrawFloat3("Scale", transform.Scale, 78.0f, 1.0f);
        });

        SerializeComponent<TransformComponent>([&](Writer& w, void* component)
        {
            const auto& transform = ComponentCast<TransformComponent>(component);
            
            w.Key("X", 1);
            w.Double(transform.Position.x);
        });

        DeserializeComponent<TransformComponent>([&](const Reader& v, void* component)
        {
            auto& transform = ComponentCast<TransformComponent>(component);
            transform.Position.x = v["X"].GetFloat();
        });

        InspectComponent<OrthographicCameraComponent>("Orthographic Camera", [&](void* component)
        {
            Scene& scene = Scene::GetActiveScene();

            auto& camera = ComponentCast<OrthographicCameraComponent>(component);

            static bool isPrimary;
            isPrimary = editorState.SelectedEntityTag == scene.PrimaryCameraTag;

            if (ImGuiWidgets::DrawBool("Primary", isPrimary, 70.0f))
            {
                scene.PrimaryCameraTag = editorState.SelectedEntityTag;
            }

            ImGuiWidgets::DrawBool("Static", camera.Static, 70.0f);

            ImGui::Text("");

            bool changed = false;

            changed |= ImGuiWidgets::DrawFloat("Aspect Ratio", camera.AspectRatio, 100.0f);
            changed |= ImGuiWidgets::DrawFloat("Size", camera.Size, 100.0f);
            changed |= ImGuiWidgets::DrawFloat("Near", camera.Near, 100.0f);
            changed |= ImGuiWidgets::DrawFloat("Far", camera.Far, 100.0f);

            if (changed)
            {
                camera.Projection = glm::ortho(-camera.AspectRatio * camera.Size,
                                                camera.AspectRatio * camera.Size,
                                               -camera.Size,
                                                camera.Size,
                                                camera.Near,
                                                camera.Far);
            }

        });

        InspectComponent<SpriteRendererComponent>("Sprite Renderer", [&](void* component)
        {
            auto& sprite = ComponentCast<SpriteRendererComponent>(component);

            const TextureData& textureData = sprite.Texture->GetData();

            ImGui::Text(textureData.Name.c_str());

            float halfWidth = ImGui::GetContentRegionAvail().x / 2.0f;

            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, halfWidth - 72.0f);

            ImGui::Image((void*)(intptr_t)textureData.RendererID, ImVec2(72.0f, 72.0f));

            ImGui::NextColumn();

            // Texture Properties
            {
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

                ImGui::Text("Wrap X");
                ImGui::SameLine();

                if (ImGui::Combo("##Wrap X", &seletedWrapXMode, WrapModeStrings, 2))
                {
                    if (seletedWrapXMode != (int32)properties.WrapX)
                    {
                        sprite.Texture->SetWrapMode((WrapMode)seletedWrapXMode, (WrapMode)seletedWrapYMode);
                    }
                }

                ImGui::Text("Wrap Y");
                ImGui::SameLine();

                if (ImGui::Combo("##Wrap Y", &seletedWrapYMode, WrapModeStrings, 2))
                {
                    if (seletedWrapYMode != (int32)properties.WrapY)
                    {
                        sprite.Texture->SetWrapMode((WrapMode)seletedWrapXMode, (WrapMode)seletedWrapYMode);
                    }
                }

                ImGui::Text("FilterMode");
                ImGui::SameLine();

                if (ImGui::Combo("##Filter Mode", &seletedFilterMode, FilterModeStrings, 2))
                {
                    if (seletedFilterMode != (int32)properties.FilterMode)
                    {
                        sprite.Texture->SetFilterMode((FilterMode)seletedFilterMode);
                    }
                }
            }

            ImGui::Columns(1);

            ImGui::Text("\n");

            ImGui::Text("Color");
            ImGui::SameLine();

            ImGui::ColorEdit4("##Color", &sprite.Color.r);
            ImGui::Text("\n");

            ImGui::Text("Top Right UV   ");
            ImGui::SameLine();

            ImGui::DragFloat2("##Top Right UV", glm::value_ptr(sprite.TopRightPoint), 0.1f);

            ImGui::Text("Bottom Left UV");
            ImGui::SameLine();

            ImGui::DragFloat2("##Bottom Left UV", glm::value_ptr(sprite.BottomLeftPoint), 0.1f);
            ImGui::Text("\n");

            ImGui::Text("Flip X");
            ImGui::SameLine();
            ImGui::Checkbox("##Flip X", &sprite.FlipX);

            ImGui::Text("Flip Y");
            ImGui::SameLine();
            ImGui::Checkbox("##Flip Y", &sprite.FlipY);
        });
	}

	void InspectorPanel::OnImGuiRender()
	{
		Scene& scene = Scene::GetActiveScene();
        EditorState& editorState = Scene::EditorState;

		ImGui::Begin("Inspector");

        Entity seletedEntity = scene.QueryEntity(editorState.SelectedEntityTag);
        const EntityStorage& storage = scene.m_Entities[seletedEntity];
        
		if (seletedEntity)
		{
            auto& tagComponent = scene.GetComponent<TagComponent>(seletedEntity);
            std::string& oldTag = tagComponent.Tag;

            char buffer[1024];
            memset(buffer, 0, 1024);
            memcpy(buffer, tagComponent.Tag.data(), tagComponent.Tag.size());

            if (ImGui::InputText("##Tag", buffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue) ||
                ImGui::IsItemDeactivated())
            {
                std::string newTag = buffer;

                if (scene.QueryEntity(newTag) == entity::null && !newTag.empty())
                {
                    Entity entity = scene.QueryEntity(editorState.SelectedEntityTag);

                    scene.m_Tags.emplace(newTag, entity);
                    scene.m_Tags.erase(oldTag);

                    if (oldTag == editorState.SelectedEntityTag)
                    {
                        editorState.SelectedEntityTag = newTag;
                    }

                    if (oldTag == scene.PrimaryCameraTag)
                    {
                        scene.PrimaryCameraTag = newTag;
                    }

                    oldTag = newTag;
                }
            }

            // Add Component Popup
            {
                ImGui::SameLine();
                ImGui::PushItemWidth(-1);

                if (ImGui::Button("Add Component"))
                {
                    if (storage.size() < editorState.ComponentMeta.size())
                    {
                        ImGui::OpenPopup("Add Component Popup");
                    }
                }

                if (ImGui::BeginPopup("Add Component Popup"))
                {
                    for (const auto& [componentID, componentInfo] : editorState.ComponentMeta)
                    {
                        if (storage.find(componentID) != storage.end())
                        {
                            continue;
                        }

                        const char* name = componentInfo.Name.c_str();

                        if (ImGui::MenuItem(name))
                        {
                            componentInfo.AddFn(seletedEntity);
                            ImGui::CloseCurrentPopup();
                        }
                    }

                    ImGui::EndPopup();
                }
            }

            ImGui::PopItemWidth();

            DrawComponents(seletedEntity, storage);
		}

		ImGui::End();
	}

    void InspectorPanel::DrawComponents(Entity entity, const EntityStorage& storage)
    {
        Scene& scene = Scene::GetActiveScene();
        EditorState& editorState = Scene::EditorState;

        auto entityStorageIter = storage.begin();

        while (entityStorageIter != storage.end())
        {
            ComponentID componentID = entityStorageIter->first;
            ComponentIndex componentIndex = entityStorageIter->second;

            entityStorageIter++;

            const ComponentInfo& info = editorState.ComponentMeta[componentID];

            if (!info.InspectFn)
            {
                continue;
            }

            ComponentState& state = m_ComponentState[{ entity, componentID }];
            const char* name = info.Name.c_str();

            const ImGuiTreeNodeFlags flags = 
                ImGuiTreeNodeFlags_DefaultOpen | 
                ImGuiTreeNodeFlags_Framed | 
                ImGuiTreeNodeFlags_FramePadding | 
                ImGuiTreeNodeFlags_SpanAvailWidth | 
                ImGuiTreeNodeFlags_AllowItemOverlap;
            
            ImVec2 contentRegionAvail = ImGui::GetContentRegionAvail();

            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2{ 4, 4 });
            float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
            ImGui::Separator();
            ImGui::SetNextTreeNodeOpen(state.Open);
            state.Open = ImGui::TreeNodeEx((void*)(intptr_t)componentID, flags, name);
            ImGui::PopStyleVar();
            
            // @Note: don't show for tag for now until we have more component setttings
            if (info.Removable)
            {
                ImGui::SameLine(contentRegionAvail.x - lineHeight * 0.5f);

                if (ImGui::Button("+", { lineHeight, lineHeight }))
                {
                    ImGui::OpenPopup("ComponentSettings");
                }
            }

            if (ImGui::BeginPopup("ComponentSettings"))
            {
                if (ImGui::Selectable("Remove Component"))
                {
                    m_ComponentState[std::make_pair(entity, componentID)] = ComponentState();
                    scene.RemoveComponent(entity, componentID);
                }

                ImGui::EndPopup();
            }
            
            if (state.Open)
            {
                void* componentMemory = scene.GetComponent(componentID, componentIndex);
                info.InspectFn(componentMemory);

                ImGui::TreePop();
            }
        }
    }

}