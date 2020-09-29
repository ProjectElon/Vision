#include "pch.h"
#include "Vision/Entity/Components.h"
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
    ShowInInspector(TagComponent)
    {
        static char Tag[1024] = "";

        auto& tag = component_cast<TagComponent>(component);

        bool expanded = ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding);

        uint32 flags = 0;

        if (expanded)
        {
            flags |= ComponentStateMask::Expaned;

            memcpy(Tag, tag.Tag.data(), 1024);

            if (ImGui::InputText("Tag", Tag, 1024, ImGuiInputTextFlags_EnterReturnsTrue) ||
                ImGui::IsItemDeactivated())
            {
                Scene& scene = Scene::GetActiveScene();
                scene.SelectedEntity.SetTag(Tag);
            }

            ImGui::TreePop();
        }

        return flags;
    }

    ShowInInspector(TransformComponent)
    {
        static glm::vec3 Position;
        static glm::quat Rotation;
        static glm::vec3 RotationAngles;
        static glm::vec3 Scale;
        static glm::vec3 Skew;
        static glm::vec4 Perspective;

        auto& transform = component_cast<TransformComponent>(component);

        glm::decompose(transform.Transform, Scale, Rotation, Position, Skew, Perspective);
        RotationAngles = glm::degrees(glm::eulerAngles(Rotation));

        bool expanded = ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding);

        uint32 flags = 0;

        if (ImGui::SmallButton("-"))
        {
            flags |= ComponentStateMask::Removed;
            const std::type_info& typeInfo = typeid(TransformComponent);
            VN_CORE_INFO("Removing Component: {0}", typeInfo.name());
        }

        if (expanded)
        {
            flags |= ComponentStateMask::Expaned;
            
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

            ImGui::TreePop();
        }

        return flags;
    }

    ShowInInspector(OrthographicCameraComponent)
    {
        auto& camera = component_cast<OrthographicCameraComponent>(component);

        bool expanded = ImGui::TreeNodeEx("Orthographic Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding);
        uint32 flags = 0;

        if (ImGui::Button("-"))
        {
            flags |= ComponentStateMask::Removed;
            
            const std::type_info& typeInfo = typeid(OrthographicCameraComponent);
            VN_CORE_INFO("Removing Component: {0}", typeInfo.name());
        }

        if (expanded)
        {
            flags |= ComponentStateMask::Expaned;

            auto& scene = Scene::GetActiveScene();
            auto activeCamera = scene.GetActiveCamera();
            
            if (scene.SelectedEntity != activeCamera)
            {
                if (ImGui::Button("Use as Main Camera"))
                {
                    scene.SetActiveCamera(scene.SelectedEntity);
                }
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

            ImGui::TreePop();
        }

        return flags;
    }

    ShowInInspector(SpriteComponent)
    {
        auto& sprite = component_cast<SpriteComponent>(component);

        bool expanded = ImGui::TreeNodeEx("Sprite", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding);        

        uint32 flags = 0;

        if (ImGui::Button("-"))
        {
            flags |= ComponentStateMask::Removed;
            const std::type_info& typeInfo = typeid(SpriteComponent);
            VN_CORE_INFO("Removing Component: {0}", typeInfo.name());
        }

        if (expanded)
        {
            flags |= ComponentStateMask::Expaned;

            if (ImGui::ImageButton((void*)(intptr_t)sprite.Texture->GetRendererID(), ImVec2(64, 64)))
            {
                // Todo: Edit The Texture Props Here
            }

            ImGui::ColorEdit4("Color", &sprite.Color.r);
            ImGui::InputFloat2("Bottom Left Point", &sprite.BottomLeftPoint.x);
            ImGui::InputFloat2("Top Right Point", &sprite.TopRightPoint.x);
            ImGui::Checkbox("Flip X", &sprite.FlipX);
            ImGui::Checkbox("Flip Y", &sprite.FlipY);

            ImGui::TreePop();
        }

        return flags;
    }
}