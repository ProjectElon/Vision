#include "InspectorPanel.h"
#include "Vision/Entity/EditorState.h"
#include "Vision/IO/TextSerializer.h"
#include "Vision/IO/TextDeserializer.h"

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Vision
{
    // @Clean Up: Asset System
    static std::unordered_map<std::string, Ref<Texture2D>> textures;

    // @Clean Up: Asset System
    static Ref<Texture2D> LoadTexture2D(const std::string& filepath)
    {
        auto textureIter = textures.find(filepath);

        if (textureIter == textures.end())
        {
            Ref<Texture2D> texture = Texture2D::CreateFromFile(filepath);
            textures.emplace(filepath, texture);
            return texture;
        }
        
        return textureIter->second;
    }

	InspectorPanel::InspectorPanel()
	{
        EditorState& editorState = Scene::EditorState;

        // adding tag manually

        ComponentInfo& info = editorState.ComponentMeta[typeid(TagComponent).hash_code()];
        info.Name = "Tag";
        info.Removable = false;

        // Transform
        {
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

                TextSerializer::SerializeVector3(w, "Position", transform.Position);
                TextSerializer::SerializeVector3(w, "Rotation", transform.Rotation);
                TextSerializer::SerializeVector3(w, "Scale", transform.Scale);
            });

            DeserializeComponent<TransformComponent>([&](const Reader& r, void* component)
            {
                auto& transform = ComponentCast<TransformComponent>(component);
                transform.Position = TextDeserializer::DeserializeVector3(r, "Position");
                transform.Rotation = TextDeserializer::DeserializeVector3(r, "Rotation");
                transform.Scale = TextDeserializer::DeserializeVector3(r, "Scale");
            });
        }

        // OrthographicCameraComponent
        {
            InspectComponent<OrthographicCameraComponent>("Orthographic Camera", [&](void* component)
            {
                Scene& scene = *Scene::GetActiveScene();

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

            SerializeComponent<OrthographicCameraComponent>([&](Writer& w, void* component)
            {
                const auto& camera = ComponentCast<OrthographicCameraComponent>(component);

                TextSerializer::SerializeFloat(w, "Aspect Ratio", camera.AspectRatio);
                TextSerializer::SerializeFloat(w, "Size", camera.Size);
                TextSerializer::SerializeFloat(w, "Near", camera.Near);
                TextSerializer::SerializeFloat(w, "Far", camera.Far);
                TextSerializer::SerializeBool(w, "Static", camera.Static);
            });

            DeserializeComponent<OrthographicCameraComponent>([&](const Reader& r, void* component)
            {
                auto& camera = ComponentCast<OrthographicCameraComponent>(component);

                camera.AspectRatio = TextDeserializer::DeserializeFloat(r, "Aspect Ratio");
                camera.Size = TextDeserializer::DeserializeFloat(r, "Size");
                camera.Near = TextDeserializer::DeserializeFloat(r, "Near");
                camera.Far = TextDeserializer::DeserializeFloat(r, "Far");
                camera.Static = TextDeserializer::DeserializeBool(r, "Static");

                camera.Projection = glm::ortho(-camera.AspectRatio * camera.Size,
                    camera.AspectRatio * camera.Size,
                    -camera.Size,
                    camera.Size,
                    camera.Near,
                    camera.Far);
            });
        }

        // SpriteRendererComponent
        {
            InspectComponent<SpriteRendererComponent>("Sprite Renderer", [&](void* component)
            {
                auto& sprite = ComponentCast<SpriteRendererComponent>(component);

                const TextureData& textureData = sprite.Texture->GetData();

                ImGui::Text(textureData.Name.c_str());

                float halfWidth = ImGui::GetContentRegionAvail().x / 2.0f;

                ImGui::Columns(2);

                ImGui::SetColumnWidth(0, halfWidth - 72.0f);

                if (ImGui::ImageButton((void*)(intptr_t)textureData.RendererID, ImVec2(72.0f, 72.0f), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    // @Clean Up: Asset System
                    std::string filepath = FileDialog::OpenFile("Texture", { ".jpeg", ".png", ".bmp", ".tga", ".psd" });

                    if (!filepath.empty())
                    {
                        sprite.Texture = LoadTexture2D(filepath);
                    }
                }

                ImGui::NextColumn();

                // Texture Properties
                // @Note: Should we edit a texture properties in a texture editor
                // or we should edit it per spite via a sampler ???
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

                    int32 seletedWrapXMode = (int32)properties.WrapX;
                    int32 seletedWrapYMode = (int32)properties.WrapY;
                    int32 seletedFilterMode = (int32)properties.FilterMode;

                    ImGui::Text("Wrap X");
                    ImGui::SameLine();

                    // @CleanUp: Add Combo to ImGui Widgets
                    if (ImGui::Combo("##Wrap X", &seletedWrapXMode, WrapModeStrings, 2))
                    {
                        if (seletedWrapXMode != (int32)properties.WrapX)
                        {
                            sprite.Texture->SetWrapMode((WrapMode)seletedWrapXMode, (WrapMode)seletedWrapYMode);
                        }
                    }

                    ImGui::Text("Wrap Y");
                    ImGui::SameLine();

                    // @CleanUp: Add Combo to ImGui Widgets
                    if (ImGui::Combo("##Wrap Y", &seletedWrapYMode, WrapModeStrings, 2))
                    {
                        if (seletedWrapYMode != (int32)properties.WrapY)
                        {
                            sprite.Texture->SetWrapMode((WrapMode)seletedWrapXMode, (WrapMode)seletedWrapYMode);
                        }
                    }

                    ImGui::Text("FilterMode");
                    ImGui::SameLine();

                    // @CleanUp: Add Combo to ImGui Widgets
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

                ImGuiWidgets::DrawColor("Color", sprite.Color);

                ImGui::Text("\n");

                ImGuiWidgets::DrawFloat2("Top Right UV", sprite.TopRightPoint, 120.0f, 1.0f);
                ImGuiWidgets::DrawFloat2("Bottom Left UV", sprite.BottomLeftPoint, 120.0f, 0.0f);

                ImGui::Text("\n");
                
                ImGuiWidgets::DrawBool("FlipX", sprite.FlipX, 50.0f);
                ImGuiWidgets::DrawBool("FlipY", sprite.FlipY, 50.0f);
            });

            SerializeComponent<SpriteRendererComponent>([&](Writer& w, void* component)
            {
                const auto& sprite = ComponentCast<SpriteRendererComponent>(component);

                const TextureData& data = sprite.Texture->GetData();
                const TextureProps& props = sprite.Texture->GetProperties();

                TextSerializer::SerializeString(w, "Texture Path", data.Path);
                TextSerializer::SerializeUint32(w, "Texture Wrap X", (uint32)props.WrapX);
                TextSerializer::SerializeUint32(w, "Texture Wrap Y", (uint32)props.WrapY);
                TextSerializer::SerializeUint32(w, "Texture Fliter Mode", (uint32)props.FilterMode);

                TextSerializer::SerializeVector4(w, "Color", sprite.Color);
                TextSerializer::SerializeVector2(w, "Bottom Left Point", sprite.BottomLeftPoint);
                TextSerializer::SerializeVector2(w, "Top Right Point", sprite.TopRightPoint);
                TextSerializer::SerializeBool(w, "Flip X", sprite.FlipX);
                TextSerializer::SerializeBool(w, "Flip Y", sprite.FlipY);
            });

            DeserializeComponent<SpriteRendererComponent>([&](const Reader& r, void* component)
            {
                auto& sprite = ComponentCast<SpriteRendererComponent>(component);

                // @Clean Up: Asset System
                std::string texturePath = TextDeserializer::DeserializeString(r, "Texture Path");

                if (!texturePath.empty())
                {
                    sprite.Texture = LoadTexture2D(texturePath);
                }

                uint32 wrapX = TextDeserializer::DeserializeUint32(r, "Texture Wrap X");
                uint32 wrapY = TextDeserializer::DeserializeUint32(r, "Texture Wrap Y");
                uint32 filterMode = TextDeserializer::DeserializeUint32(r, "Texture Fliter Mode");

                sprite.Texture->SetWrapMode((WrapMode)wrapX, (WrapMode)wrapY);
                sprite.Texture->SetFilterMode((FilterMode)filterMode);

                sprite.Color = TextDeserializer::DeserializeVector4(r, "Color");

                sprite.BottomLeftPoint = TextDeserializer::DeserializeVector2(r, "Bottom Left Point");
                sprite.TopRightPoint = TextDeserializer::DeserializeVector2(r, "Top Right Point");

                sprite.FlipX = TextDeserializer::DeserializeBool(r, "Flip X");
                sprite.FlipY = TextDeserializer::DeserializeBool(r, "Flip Y");
            });
        }
	}

	void InspectorPanel::OnImGuiRender()
	{
        EditorState& editorState = Scene::EditorState;
        Scene* scene = Scene::GetActiveScene();

		ImGui::Begin("Inspector");


        if (scene)
        {
            Entity seletedEntity = scene->QueryEntity(editorState.SelectedEntityTag);
            const EntityStorage& storage = scene->m_Entities[seletedEntity];

            if (seletedEntity)
            {
                auto& tagComponent = scene->GetComponent<TagComponent>(seletedEntity);
                std::string& oldTag = tagComponent.Tag;

                char buffer[1024];
                memset(buffer, 0, 1024);
                memcpy(buffer, tagComponent.Tag.data(), tagComponent.Tag.size());

                if (ImGui::InputText("##Tag", buffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue) ||
                    ImGui::IsItemDeactivated())
                {
                    std::string newTag = buffer;

                    if (scene->QueryEntity(newTag) == entity::null && !newTag.empty())
                    {
                        Entity entity = scene->QueryEntity(editorState.SelectedEntityTag);

                        scene->m_Tags.emplace(newTag, entity);
                        scene->m_Tags.erase(oldTag);

                        if (oldTag == editorState.SelectedEntityTag)
                        {
                            editorState.SelectedEntityTag = newTag;
                        }

                        if (oldTag == scene->PrimaryCameraTag)
                        {
                            scene->PrimaryCameraTag = newTag;
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

                DrawComponents(*scene, seletedEntity, storage);
            }
        }

		ImGui::End();
	}

    void InspectorPanel::DrawComponents(Scene& scene, Entity entity, const EntityStorage& storage)
    {
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