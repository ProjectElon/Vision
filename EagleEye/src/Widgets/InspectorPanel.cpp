#include "InspectorPanel.h"

#include "EagleEye.h"

#include "Vision/Platform/FileSystem.h"
#include "Vision/Assets/Assets.h"
#include "Vision/Platform/PlatformUtils.h"
#include "Vision/Entity/Components.h"
#include "Vision/ImGui/ImGuiWidgets.h"
#include "Vision/Renderer/Renderer.h"

#include <sstream>

#include <imgui.h>
#include <imgui_internal.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace Vision
{
	InspectorPanel::InspectorPanel()
	{
        EditorState& editorState = EagleEye::EditorState;

        ComponentInfo& info = editorState.ComponentMeta[typeid(TagComponent).hash_code()];
        info.Name = "Tag";
        info.Removable = false;

        InspectComponent<TransformComponent>("Transform", [&](Scene* scene, void* component)
        {
            auto& transform = ComponentCast<TransformComponent>(component);

            ImGuiWidgets::DrawFloat3("Position", (glm::vec3&)transform.Position, 78.0f, 0);

            glm::vec3 rotation = glm::degrees((glm::vec3&)transform.Rotation);

            ImGuiWidgets::DrawFloat3("Rotation", rotation, 78.0f, 0);

            transform.Rotation = glm::radians(rotation);

            ImGuiWidgets::DrawFloat3("Scale", (glm::vec3&)transform.Scale, 78.0f, 1.0f);
        });

        SerializeComponent<TransformComponent>([&](void* component)
        {
            const auto& transform = ComponentCast<TransformComponent>(component);

            const Vector3& p = transform.Position;
            const Vector3& r = transform.Rotation;
            const Vector3& s = transform.Scale;

            std::stringstream stringStream;

            stringStream << "Position " << p.x << " " << p.y << " " << p.z << "\n";
            stringStream << "Rotation " << r.x << " " << r.y << " " << r.z << "\n";
            stringStream << "Scale "    << s.x << " " << s.y << " " << s.z << "\n";

            return stringStream.str();
        });

        DeserializeComponent<TransformComponent>([&](void* component, const std::string& contents)
        {
            auto& transform = ComponentCast<TransformComponent>(component);

            Vector3& p = transform.Position;
            Vector3& r = transform.Rotation;
            Vector3& s = transform.Scale;

            std::string prop;
            std::string newline;
            std::string space;

            std::stringstream stringStream(contents);

            stringStream >> prop >> p.x >> p.y >> p.z;
            stringStream >> prop >> r.x >> r.y >> r.z;
            stringStream >> prop >> s.x >> s.y >> s.z;
        });

        InspectComponent<OrthographicCameraComponent>("Orthographic Camera", [&](Scene* scene, void* component)
        {
            auto& camera = ComponentCast<OrthographicCameraComponent>(component);

            static bool isPrimary = false;
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

        // Todo(Harlequin): Make a Serialize texture function for Sprite Renderer and Mesh Renderer

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

            if (ImGui::ImageButton(*(ImTextureID*)Renderer::ConvertTextureToImGuiTexture(texture), ImVec2(72.0f, 72.0f), ImVec2(0, 1), ImVec2(1, 0)))
            {
                std::string texturepath = FileDialog::OpenFile("Texture", { "png", "jpeg", "psd", "bmp", "tga" });

                if (!texturepath.empty())
                {
                    Assets::ReleaseAsset(sprite.Texture);
                    sprite.Texture = Assets::RequestAsset(texturepath);
                    texture = Assets::GetTexture(sprite.Texture);
                }
            }

            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content_Browser_Item");

                if (payload)
                {
                    std::string path = std::string((const char*)payload->Data);
                    std::string extension = FileSystem::GetFileExtension(path, false);

                    if (Assets::IsExtensionOfAssetType(extension, "Texture"))
                    {
                        Asset textureAsset = Assets::GetAsset(sprite.Texture);

                        if (textureAsset.Path != path)
                        {
                            Assets::ReleaseAsset(sprite.Texture);
                            sprite.Texture = Assets::RequestAsset(path);
                            texture = Assets::GetTexture(sprite.Texture);
                        }
                    }
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::NextColumn();

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

            auto seletedWrapXMode  = static_cast<int32>(texture->WrapX);
            auto seletedWrapYMode  = static_cast<int32>(texture->WrapY);
            auto seletedFilterMode = static_cast<int32>(texture->Filter);

            ImGui::Text("Wrap X");
            ImGui::SameLine();

            if (ImGui::Combo("##Wrap X", &seletedWrapXMode, WrapModeStrings, 2))
            {
                if (seletedWrapXMode != static_cast<int32>(texture->WrapX))
                {
                    Renderer::SetTextureWrapMode(texture,
                                                 static_cast<WrapMode>(seletedWrapXMode),
                                                 static_cast<WrapMode>(seletedWrapYMode));
                }
            }

            ImGui::Text("Wrap Y");
            ImGui::SameLine();

            // @CleanUp: Add Combo to ImGui Widgets
            if (ImGui::Combo("##Wrap Y", &seletedWrapYMode, WrapModeStrings, 2))
            {
                if (seletedWrapYMode != static_cast<int32>(texture->WrapY))
                {
                    Renderer::SetTextureWrapMode(texture,
                                                 static_cast<WrapMode>(seletedWrapXMode),
                                                 static_cast<WrapMode>(seletedWrapYMode));
                }
            }

            ImGui::Text("FilterMode");
            ImGui::SameLine();

            if (ImGui::Combo("##Filter Mode", &seletedFilterMode, FilterModeStrings, 2))
            {
                if (seletedFilterMode != static_cast<int32>(texture->Filter))
                {
                    Renderer::SetTextureFilterMode(texture, static_cast<FilterMode>(seletedFilterMode));
                }
            }

            ImGui::Columns(1);

            ImGui::Text("\n");

            ImGuiWidgets::DrawColor("Color", (glm::vec4&)sprite.Color);

            ImGui::Text("\n");

            ImGuiWidgets::DrawFloat2("Top Right UV",   (glm::vec2&)sprite.TopRightUV, 120.0f, 1.0f);
            ImGuiWidgets::DrawFloat2("Bottom Left UV", (glm::vec2&)sprite.BottomLeftUV, 120.0f, 0.0f);
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
            stringStream << "Texture Fliter Mode " << (uint32)texture->Filter << "\n";

            const Vector4& c = sprite.Color;
            const Vector2& bl = sprite.BottomLeftUV;
            const Vector2& tr = sprite.TopRightUV;

            stringStream << "Color " << c.x << " " << c.y << " " << c.z << " " << c.w << "\n";
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

            uint32 wrapX;
            uint32 wrapY;
            uint32 filter;

            stringStream >> prop >> prop >> prop >> wrapX;
            stringStream >> prop >> prop >> prop >> wrapY;
            stringStream >> prop >> prop >> prop >> filter;

            Renderer::SetTextureWrapMode(texture,
                                         static_cast<WrapMode>(wrapX),
                                         static_cast<WrapMode>(wrapY));

            Renderer::SetTextureFilterMode(texture, static_cast<FilterMode>(filter));

            Vector4& c = sprite.Color;
            Vector2& bl = sprite.BottomLeftUV;
            Vector2& tr = sprite.TopRightUV;

            stringStream >> prop >> c.x  >> c.y  >> c.z  >> c.w;
            stringStream >> prop >> prop >> prop >> bl.x >> bl.y;
            stringStream >> prop >> prop >> prop >> tr.x >> tr.y;
        });

         InspectComponent<MeshRendererComponent>("Mesh Renderer", [&](Scene* scene, void* component)
         {
            auto& meshComponent = ComponentCast<MeshRendererComponent>(component);

            const Asset& meshAsset = Assets::GetAsset(meshComponent.MeshAssetID);

            ImGui::Text("Mesh: %s", meshAsset.Path.c_str());

            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content_Browser_Item");

                if (payload)
                {
                    std::string path = std::string((const char*)payload->Data);
                    std::string extension = FileSystem::GetFileExtension(path, false);

                    if (Assets::IsExtensionOfAssetType(extension, "Mesh"))
                    {
                        if (meshAsset.Path != path)
                        {
                            Assets::ReleaseAsset(meshComponent.MeshAssetID);
                            meshComponent.MeshAssetID = Assets::RequestAsset(path);
                        }
                    }
                }

                ImGui::EndDragDropTarget();
            }

            const Asset& textureAsset = Assets::GetAsset(meshComponent.TextureAssetID);

            Texture* texture = Assets::GetTexture(meshComponent.TextureAssetID);

            std::string textureName = FileSystem::GetFileName(textureAsset.Path);

            ImGui::Text(textureName.c_str());

            float halfWidth = ImGui::GetContentRegionAvail().x / 2.0f;

            ImGui::Columns(2);

            ImGui::SetColumnWidth(0, halfWidth - 72.0f);

            if (ImGui::ImageButton(*(ImTextureID*)Renderer::ConvertTextureToImGuiTexture(texture), ImVec2(72.0f, 72.0f), ImVec2(0, 1), ImVec2(1, 0)))
            {
                std::string texturepath = FileDialog::OpenFile("Texture", { "png", "jpeg", "psd", "bmp", "tga" });

                if (!texturepath.empty() && texturepath != textureAsset.Path)
                {
                    Assets::ReleaseAsset(meshComponent.TextureAssetID);
                    meshComponent.TextureAssetID = Assets::RequestAsset(texturepath);
                    texture = Assets::GetTexture(meshComponent.TextureAssetID);
                }
            }

            if (ImGui::BeginDragDropTarget())
            {
                const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("Content_Browser_Item");

                if (payload)
                {
                    std::string path = std::string((const char*)payload->Data);
                    std::string extension = FileSystem::GetFileExtension(path, false);

                    if (Assets::IsExtensionOfAssetType(extension, "Texture"))
                    {
                        Asset textureAsset = Assets::GetAsset(meshComponent.TextureAssetID);

                        if (textureAsset.Path != path)
                        {
                            Assets::ReleaseAsset(meshComponent.TextureAssetID);
                            meshComponent.TextureAssetID = Assets::RequestAsset(path);
                            texture = Assets::GetTexture(meshComponent.TextureAssetID);
                        }
                    }
                }

                ImGui::EndDragDropTarget();
            }

            ImGui::NextColumn();

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

            auto seletedWrapXMode  = static_cast<int32>(texture->WrapX);
            auto seletedWrapYMode  = static_cast<int32>(texture->WrapY);
            auto seletedFilterMode = static_cast<int32>(texture->Filter);

            ImGui::Text("Wrap X");
            ImGui::SameLine();

            if (ImGui::Combo("##Wrap X", &seletedWrapXMode, WrapModeStrings, 2))
            {
                if (seletedWrapXMode != static_cast<int32>(texture->WrapX))
                {
                    Renderer::SetTextureWrapMode(texture,
                                                 static_cast<WrapMode>(seletedWrapXMode),
                                                 static_cast<WrapMode>(seletedWrapYMode));
                }
            }

            ImGui::Text("Wrap Y");
            ImGui::SameLine();

            // @CleanUp: Add Combo to ImGui Widgets
            if (ImGui::Combo("##Wrap Y", &seletedWrapYMode, WrapModeStrings, 2))
            {
                if (seletedWrapYMode != static_cast<int32>(texture->WrapY))
                {
                    Renderer::SetTextureWrapMode(texture,
                                                 static_cast<WrapMode>(seletedWrapXMode),
                                                 static_cast<WrapMode>(seletedWrapYMode));
                }
            }

            ImGui::Text("FilterMode");
            ImGui::SameLine();

            if (ImGui::Combo("##Filter Mode", &seletedFilterMode, FilterModeStrings, 2))
            {
                if (seletedFilterMode != static_cast<int32>(texture->Filter))
                {
                    Renderer::SetTextureFilterMode(texture, static_cast<FilterMode>(seletedFilterMode));
                }
            }
        });

        SerializeComponent<MeshRendererComponent>([&](void* component)
        {
            const auto& mesh = ComponentCast<MeshRendererComponent>(component);

            std::stringstream stringStream;

            const Asset& meshAsset = Assets::GetAsset(mesh.MeshAssetID);
            stringStream << "Mesh Path " << meshAsset.Path << "\n";

            const Asset& textureAsset = Assets::GetAsset(mesh.TextureAssetID);
            Texture* texture = Assets::GetTexture(mesh.TextureAssetID);

            stringStream << "Texture Path " << textureAsset.Path << "\n";
            stringStream << "Texture Wrap X " << (uint32)texture->WrapX << "\n";
            stringStream << "Texture Wrap Y " << (uint32)texture->WrapY << "\n";
            stringStream << "Texture Fliter Mode " << (uint32)texture->Filter << "\n";

            return stringStream.str();
        });

        DeserializeComponent<MeshRendererComponent>([&](void* component, const std::string& contents)
        {
            auto& mesh = ComponentCast<MeshRendererComponent>(component);

            std::string prop;

            std::stringstream stringStream(contents);

            std::string meshPath;
            stringStream >> prop >> prop >> meshPath;
            mesh.MeshAssetID = Assets::RequestAsset(meshPath);

            std::string texturePath;
            stringStream >> prop >> prop >> texturePath;

            mesh.TextureAssetID = Assets::RequestAsset(texturePath);
            Texture* texture = Assets::GetTexture(mesh.TextureAssetID);

            uint32 wrapX;
            uint32 wrapY;
            uint32 filter;

            stringStream >> prop >> prop >> prop >> wrapX;
            stringStream >> prop >> prop >> prop >> wrapY;
            stringStream >> prop >> prop >> prop >> filter;

            Renderer::SetTextureWrapMode(texture,
                static_cast<WrapMode>(wrapX),
                static_cast<WrapMode>(wrapY));

            Renderer::SetTextureFilterMode(texture, static_cast<FilterMode>(filter));
        });
	}

	void InspectorPanel::OnImGuiRender()
	{
        EditorState& editorState = EagleEye::EditorState;

		ImGui::Begin("Inspector");

        if (ActiveSceneID)
        {
            Scene* scene = Assets::GetScene(ActiveSceneID);

            Entity seletedEntity = scene->QueryEntity(editorState.SelectedEntityTag);

            if (seletedEntity)
            {
                const EntityStorage& entityStorage = scene->Entities[seletedEntity];

                auto& tagComponent = scene->GetComponent<TagComponent>(seletedEntity);
                std::string oldTag = tagComponent.Tag.Data;

                char buffer[MaxEntityNameCount];
                memset(buffer, 0, sizeof(buffer));
                strcpy(buffer, tagComponent.Tag.Data);

                if (ImGui::InputText("##Tag",
                                     buffer,
                                     sizeof(buffer),
                                     ImGuiInputTextFlags_EnterReturnsTrue) ||
                                     ImGui::IsItemDeactivated())
                {
                    std::string newTag = std::string(buffer);

                    bool isNewTagAvailable = scene->QueryEntity(newTag) == 0;

                    if (!newTag.empty() && newTag != "none" && isNewTagAvailable)
                    {
                        Entity entity = scene->QueryEntity(editorState.SelectedEntityTag);

                        scene->Tags.erase(oldTag);
                        scene->Tags.emplace(newTag, entity);

                        if (oldTag == editorState.SelectedEntityTag)
                        {
                            editorState.SelectedEntityTag = newTag;
                        }

                        if (oldTag == scene->PrimaryCameraTag)
                        {
                            scene->PrimaryCameraTag = newTag;
                        }

                        AssignString(tagComponent.Tag, newTag.c_str(), newTag.length());
                    }
                }


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

                ImGui::PopItemWidth();

                DrawComponents(scene, seletedEntity, entityStorage);
            }
        }

		ImGui::End();
	}

    void InspectorPanel::DrawComponents(Scene* scene, Entity entity, const EntityStorage& storage)
    {
        EditorState& editorState = EagleEye::EditorState;

        auto entityStorageIter = storage.begin();

        while (entityStorageIter != storage.end())
        {
            ComponentID componentID = entityStorageIter->first;
            ComponentIndex componentIndex = entityStorageIter->second;

            entityStorageIter++;

            const ComponentInfo& info = editorState.ComponentMeta[componentID];
            std::string tag = scene->GetComponent<TagComponent>(entity).Tag.Data;

            if (!info.InspectFn)
            {
                continue;
            }

            Vision::ComponentState& state = ComponentState[{ tag, componentID }];
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
                    ComponentState[std::make_pair(tag, componentID)] = Vision::ComponentState();
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