#include "InspectorPanel.h"

#include "Vision/Entity/EditorState.h"
#include "Vision/IO/Assets.h"
#include "Vision/Entity/Scene.h"

#include <sstream>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Vision
{
	InspectorPanel::InspectorPanel()
	{
        EditorState& editorState = Scene::EditorState;

        // Tag Component
        {
            ComponentInfo& info = editorState.ComponentMeta[typeid(TagComponent).hash_code()];
            info.Name = "Tag";
            info.Removable = false;
        }

        // Transform Component
        {
            InspectComponent<TransformComponent>("Transform", [&](Scene* scene, void* component)
            {
                auto& transform = ComponentCast<TransformComponent>(component);

                ImGuiWidgets::DrawFloat3("Position", transform.Position, 78.0f, 0);

                glm::vec3 rotation = glm::degrees(transform.Rotation);

                ImGuiWidgets::DrawFloat3("Rotation", rotation, 78.0f, 0);

                transform.Rotation = glm::radians(rotation);

                ImGuiWidgets::DrawFloat3("Scale", transform.Scale, 78.0f, 1.0f);
            });

            SerializeComponent<TransformComponent>([&](void* component)
            {
                const auto& transform = ComponentCast<TransformComponent>(component);

                const glm::vec3& p = transform.Position;
                const glm::vec3& r = transform.Rotation;
                const glm::vec3& s = transform.Scale;

                std::stringstream stringStream;

                stringStream << "Position " << p.x << " " << p.y << " " << p.z << "\n";
                stringStream << "Rotation " << r.x << " " << r.y << " " << r.z << "\n";
                stringStream << "Scale "    << s.x << " " << s.y << " " << s.z << "\n";

                return stringStream.str();
            });

            DeserializeComponent<TransformComponent>([&](void* component, const std::string& contents)
            {
                auto& transform = ComponentCast<TransformComponent>(component);

                glm::vec3& p = transform.Position;
                glm::vec3& r = transform.Rotation;
                glm::vec3& s = transform.Scale;

                std::string prop;
                std::string newline;
                std::string space;

                std::stringstream stringStream(contents);

                stringStream >> prop >> p.x >> p.y >> p.z;
                stringStream >> prop >> r.x >> r.y >> r.z;
                stringStream >> prop >> s.x >> s.y >> s.z;
            });
        }

        // OrthographicCameraComponent
        {
            InspectComponent<OrthographicCameraComponent>("Orthographic Camera", [&](Scene* scene, void* component)
            {
                auto& camera = ComponentCast<OrthographicCameraComponent>(component);

                static bool isPrimary;
                isPrimary = editorState.SelectedEntityTag == scene->PrimaryCameraTag;

                if (ImGuiWidgets::DrawBool("Primary", isPrimary, 70.0f))
                {
                    scene->PrimaryCameraTag = editorState.SelectedEntityTag;
                }

                ImGuiWidgets::DrawBool("Static", camera.Static, 70.0f);

                ImGui::Text("\n");

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

            SerializeComponent<OrthographicCameraComponent>([&](void* component)
            {
                const auto& camera = ComponentCast<OrthographicCameraComponent>(component);

                std::stringstream stringStream;

                stringStream << "Aspect Ratio " << camera.AspectRatio << "\n";
                stringStream << "Size " << camera.Size << "\n";
                stringStream << "Near " << camera.Near << "\n";
                stringStream << "Far " << camera.Far << "\n";
                stringStream << "Static " << camera.Static << "\n";

                return stringStream.str();
            });

            DeserializeComponent<OrthographicCameraComponent>([&](void* component, const std::string& contents)
            {
                auto& camera = ComponentCast<OrthographicCameraComponent>(component);

                std::string prop;

                std::stringstream stringStream(contents);

                stringStream >> prop >> camera.AspectRatio;
                stringStream >> prop >> camera.Size;
                stringStream >> prop >> camera.Near;
                stringStream >> prop >> camera.Far;
                stringStream >> prop >> camera.Static;
            });
        }

        // SpriteRendererComponent
        {
            InspectComponent<SpriteRendererComponent>("Sprite Renderer", [&](Scene* scene, void* component)
            {
                auto& sprite = ComponentCast<SpriteRendererComponent>(component);

                const Asset& textureAsset = Assets::GetAsset(sprite.Texture);

                Texture* texture = Assets::GetTexture(sprite.Texture);

                std::string textureName = FileSystem::GetFileName(textureAsset.Path);

                ImGui::Text(textureName.c_str());

                float halfWidth = ImGui::GetContentRegionAvail().x / 2.0f;

                ImGui::Columns(2);

                ImGui::SetColumnWidth(0, halfWidth - 72.0f);

                if (ImGui::ImageButton((void*)(intptr_t)texture->RendererID, ImVec2(72.0f, 72.0f), ImVec2(0, 1), ImVec2(1, 0)))
                {
                    std::string texturepath = FileDialog::OpenFile("Texture", { "png", "jpeg", "psd", "bmp", "tga" });

                    if (!texturepath.empty())
                    {
                        Assets::ReleaseAsset(sprite.Texture);
                        sprite.Texture = Assets::RequestAsset(texturepath);
                        texture = Assets::GetTexture(sprite.Texture);
                    }
                }

                ImGui::NextColumn();

                // Texture Properties
                // @Note: Should we edit a texture properties in a texture editor
                // or we should edit it per spite via a sampler ???
                {
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

                    int32 seletedWrapXMode = (int32)texture->WrapX;
                    int32 seletedWrapYMode = (int32)texture->WrapY;
                    int32 seletedFilterMode = (int32)texture->FilterMode;

                    ImGui::Text("Wrap X");
                    ImGui::SameLine();

                    // @CleanUp: Add Combo to ImGui Widgets
                    if (ImGui::Combo("##Wrap X", &seletedWrapXMode, WrapModeStrings, 2))
                    {
                        if (seletedWrapXMode != (int32)texture->WrapX)
                        {
                            SetTextureWrapMode(texture,
                                               (WrapMode)seletedWrapXMode,
                                               (WrapMode)seletedWrapYMode);
                        }
                    }

                    ImGui::Text("Wrap Y");
                    ImGui::SameLine();

                    // @CleanUp: Add Combo to ImGui Widgets
                    if (ImGui::Combo("##Wrap Y", &seletedWrapYMode, WrapModeStrings, 2))
                    {
                        if (seletedWrapYMode != (int32)texture->WrapY)
                        {
                            SetTextureWrapMode(texture,
                                               (WrapMode)seletedWrapXMode,
                                               (WrapMode)seletedWrapYMode);
                        }
                    }

                    ImGui::Text("FilterMode");
                    ImGui::SameLine();

                    // @CleanUp: Add Combo to ImGui Widgets
                    if (ImGui::Combo("##Filter Mode", &seletedFilterMode, FilterModeStrings, 2))
                    {
                        if (seletedFilterMode != (int32)texture->FilterMode)
                        {
                            SetTextureFilterMode(texture, (FilterMode)seletedFilterMode);
                        }
                    }
                }

                ImGui::Columns(1);

                ImGui::Text("\n");

                ImGuiWidgets::DrawColor("Color", sprite.Color);

                ImGui::Text("\n");

                ImGuiWidgets::DrawFloat2("Top Right UV", sprite.TopRightUV, 120.0f, 1.0f);
                ImGuiWidgets::DrawFloat2("Bottom Left UV", sprite.BottomLeftUV, 120.0f, 0.0f);
            });

            SerializeComponent<SpriteRendererComponent>([&](void* component)
            {
                const auto& sprite = ComponentCast<SpriteRendererComponent>(component);

                std::stringstream stringStream;

                const Asset& textureAsset = Assets::GetAsset(sprite.Texture);
                Texture* texture = Assets::GetTexture(sprite.Texture);

                stringStream << "Texture Path " << textureAsset.Path << "\n";
                stringStream << "Texture Wrap X " << (uint32)texture->WrapX << "\n";
                stringStream << "Texture Wrap Y " << (uint32)texture->WrapY << "\n";
                stringStream << "Texture Fliter Mode " << (uint32)texture->FilterMode << "\n";

                const glm::vec4& c = sprite.Color;
                const glm::vec2& bl = sprite.BottomLeftUV;
                const glm::vec2& tr = sprite.TopRightUV;

                stringStream << "Color " << c.r << " " << c.g << " " << c.b << " " << c.a << "\n";
                stringStream << "Bottom Left Point " << bl.x << " " << bl.y << "\n";
                stringStream << "Top Right Point " << tr.x << " " << tr.y  << "\n";

                return stringStream.str();
            });

            DeserializeComponent<SpriteRendererComponent>([&](void* component, const std::string& contents)
            {
                auto& sprite = ComponentCast<SpriteRendererComponent>(component);

                std::string prop;

                std::stringstream stringStream(contents);

                std::string texturePath;
                stringStream >> prop >> prop >> texturePath;

                sprite.Texture = Assets::RequestAsset(texturePath);
                Texture* texture = Assets::GetTexture(sprite.Texture);

                stringStream >> prop >> prop >> prop >> (uint32&)texture->WrapX;
                stringStream >> prop >> prop >> prop >> (uint32&)texture->WrapY;
                stringStream >> prop >> prop >> prop >> (uint32&)texture->FilterMode;

                SetTextureWrapMode(texture, texture->WrapX, texture->WrapY);
                SetTextureFilterMode(texture, texture->FilterMode);

                glm::vec4& c = sprite.Color;
                glm::vec2& bl = sprite.BottomLeftUV;
                glm::vec2& tr = sprite.TopRightUV;

                stringStream >> prop >> c.r  >> c.g  >> c.b  >> c.a;
                stringStream >> prop >> prop >> prop >> bl.x >> bl.y;
                stringStream >> prop >> prop >> prop >> tr.x >> tr.y;
            });
        }
	}

    void InspectorPanel::SetActiveScene(AssetID scene)
    {
        m_ActiveScene = scene;
    }

	void InspectorPanel::OnImGuiRender()
	{
        EditorState& editorState = Scene::EditorState;

		ImGui::Begin("Inspector");

        if (m_ActiveScene)
        {
            Scene* scene = Assets::GetScene(m_ActiveScene);

            Entity seletedEntity = scene->QueryEntity(editorState.SelectedEntityTag);

            if (seletedEntity != entity::null)
            {
                const EntityStorage& entityStorage = scene->Entities[seletedEntity];

                auto& tagComponent = scene->GetComponent<TagComponent>(seletedEntity);
                std::string oldTag = tagComponent.Tag;

                char buffer[MaxEntityNameCount];
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, tagComponent.Tag);

                if (ImGui::InputText("##Tag",
                                     buffer,
                                     sizeof(buffer),
                                     ImGuiInputTextFlags_EnterReturnsTrue) ||
                                     ImGui::IsItemDeactivated())
                {
                    std::string newTag = std::string(buffer);

                    bool isNewTagAvailable = scene->QueryEntity(newTag) == entity::null;

                    if (!newTag.empty() && newTag != "none" && isNewTagAvailable)
                    {
                        Entity entity = scene->QueryEntity(editorState.SelectedEntityTag);

                        scene->Tags.emplace(newTag, entity);
                        scene->Tags.erase(oldTag);

                        if (oldTag == editorState.SelectedEntityTag)
                        {
                            editorState.SelectedEntityTag = newTag;
                        }

                        if (oldTag == scene->PrimaryCameraTag)
                        {
                            scene->PrimaryCameraTag = newTag;
                        }

                        strcpy(tagComponent.Tag, buffer);
                        // tagComponent.Tag = newTag;
                    }
                }

                // Add Component Popup
                {
                    ImGui::SameLine();
                    ImGui::PushItemWidth(-1);

                    if (ImGui::Button("Add Component"))
                    {
                        if (entityStorage.size() < editorState.ComponentMeta.size())
                        {
                            ImGui::OpenPopup("Add Component Popup");
                        }
                    }

                    if (ImGui::BeginPopup("Add Component Popup"))
                    {
                        for (const auto& [componentID, componentInfo] : editorState.ComponentMeta)
                        {
                            bool isComponentFound = entityStorage.find(componentID) != entityStorage.end();

                            if (isComponentFound)
                            {
                                continue;
                            }

                            const char* name = componentInfo.Name.c_str();

                            if (ImGui::MenuItem(name))
                            {
                                componentInfo.AddFn(scene, seletedEntity);
                                ImGui::CloseCurrentPopup();
                            }
                        }

                        ImGui::EndPopup();
                    }
                }

                ImGui::PopItemWidth();

                DrawComponents(scene, seletedEntity, entityStorage);
            }
        }

		ImGui::End();
	}

    void InspectorPanel::DrawComponents(Scene* scene, Entity entity, const EntityStorage& storage)
    {
        EditorState& editorState = Scene::EditorState;

        auto entityStorageIter = storage.begin();

        while (entityStorageIter != storage.end())
        {
            ComponentID componentID = entityStorageIter->first;
            ComponentIndex componentIndex = entityStorageIter->second;

            entityStorageIter++;

            const ComponentInfo& info = editorState.ComponentMeta[componentID];
            const std::string& tag = scene->GetComponent<TagComponent>(entity).Tag;

            if (!info.InspectFn)
            {
                continue;
            }

            ComponentState& state = m_ComponentState[{ tag, componentID }];
            const char* name = info.Name.c_str();

            ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_DefaultOpen |
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
                    m_ComponentState[std::make_pair(tag, componentID)] = ComponentState();
                    scene->RemoveComponent(entity, componentID);
                }

                ImGui::EndPopup();
            }

            if (state.Open)
            {
                void* componentMemory = scene->GetComponent(componentID, componentIndex);
                info.InspectFn(scene, componentMemory);

                ImGui::TreePop();
            }
        }
    }

}