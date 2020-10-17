#include "InspectorPanel.h"
#include "Vision/Entity/EditorState.h"

#include <imgui.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Vision
{
	InspectorPanel::InspectorPanel()
	{
		m_ComponentsToRemove.reserve(10);

        auto& editorState = Scene::EditorState;
        
		InspectComponent<TagComponent, false>("Tag", [&](void* component)
		{
            char buffer[1024];

            Scene& scene = Scene::GetActiveScene();
			auto& tagComponent = ComponentCast<TagComponent>(component);
            std::string& oldTag = tagComponent.Tag;

            memset(buffer, 0, 1024);
			memcpy(buffer, tagComponent.Tag.data(), tagComponent.Tag.size());

			if (ImGui::InputText("Tag", buffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue) ||
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
            static glm::vec3 Position;
            static glm::quat Rotation;
            static glm::vec3 RotationAngles;
            static glm::vec3 Scale;
            static glm::vec3 Skew;
            static glm::vec4 Perspective;

            auto& transform = ComponentCast<TransformComponent>(component);

            glm::decompose(transform.Transform, Scale, Rotation, Position, Skew, Perspective);
            RotationAngles = glm::degrees(glm::eulerAngles(Rotation));

            bool changed = false;

            changed |= ImGui::InputFloat3("Position ", &Position.x);
            changed |= ImGui::InputFloat3("Rotation ", &RotationAngles.x);
            changed |= ImGui::InputFloat3("Scale ", &Scale.x);

            Rotation = glm::quat(glm::radians(RotationAngles));

            if (changed)
            {
                transform.Transform =
                    glm::translate(glm::mat4(1.0f), Position) *
                    glm::mat4_cast(Rotation) *
                    glm::scale(glm::mat4(1.0f), Scale);
            }
        });

        InspectComponent<OrthographicCameraComponent>("Orthographic Camera", [&](void* component)
        {
            Scene& scene = Scene::GetActiveScene();
            
            auto& camera = ComponentCast<OrthographicCameraComponent>(component);

            if (editorState.SeleteEntityTag != scene.ActiveCameraTag)
            {
                if (ImGui::Button("Set as Primary"))
                {
                    scene.ActiveCameraTag = editorState.SeleteEntityTag;
                }
            }
            else
            {
                ImGui::TextColored({ 0.931f, 0.618f, 0.265f, 1.0f }, "Primary Camera");
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
            
            for (const auto& [componentID, componentIndex] : storage)
            {
                ComponentInfo& componentInfo = componentInspector[componentID];
                
                const ComponentInspectFn& inspectFn = componentInfo.InspectFn;
                
                if (!inspectFn || componentIndex == -1)
                {
                    continue;
                }

                void* component = scene.GetComponentMemory(componentID, componentIndex);
                
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
                        componentState.PendingRemove = true;
                        m_ComponentsToRemove.emplace_back(componentID);
                    }
                }

                if (componentState.Expanded)
                {
                    inspectFn(component);
                    ImGui::TreePop();
                }

                ImGui::Text("\n");
            }

			if (!m_ComponentsToRemove.empty())
			{
				for (auto& componentID : m_ComponentsToRemove)
				{
					scene.RemoveComponent(seletedEntity, componentID);
                    m_ComponentState[{ seletedEntity, componentID }] = ComponentState();
				}

				m_ComponentsToRemove.resize(0);
			}

			if (ImGui::Button("Add Components"))
			{
                if (storage.size() < componentInspector.size())
                {
                    ImGui::OpenPopup("Components");
                }
			}

			if (ImGui::BeginPopup("Components"))
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