#include "pch.h"
#include "Components.h"

namespace Vision
{
    ShowInInspector(TagComponent)
    {
        static char StringBuffer[1024] = "";

        auto& tag = component_cast<TagComponent>(component);

        if (ImGui::TreeNodeEx("Tag", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
        {
            memcpy(StringBuffer, tag.Tag.data(), 1024);

            if (ImGui::InputText("Tag", StringBuffer, 1024, ImGuiInputTextFlags_EnterReturnsTrue) ||
                ImGui::IsItemDeactivated())
            {
                // Todo: check if tag is unique
                tag.Tag = StringBuffer;
            }

            ImGui::TreePop();
        }
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

        if (ImGui::TreeNodeEx("Transform", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
        {
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
    }

    ShowInInspector(OrthographicCameraComponent)
    {
        auto& camera = component_cast<OrthographicCameraComponent>(component);

        if (ImGui::TreeNodeEx("Orthographic Camera", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
        {
            bool changed = false;

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
    }

    ShowInInspector(SpriteComponent)
    {
        auto& sprite = component_cast<SpriteComponent>(component);

        if (ImGui::TreeNodeEx("Sprite", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
        {
            ImGui::Image((void*)(intptr_t)sprite.Texture->GetRendererID(), ImVec2(32, 32));
            ImGui::ColorEdit4("Color", &sprite.Color.r);
            ImGui::InputFloat2("Bottom Left Point", &sprite.BottomLeftPoint.x);
            ImGui::InputFloat2("Top Right Point", &sprite.TopRightPoint.x);
            ImGui::Checkbox("Flip X", &sprite.FlipX);
            ImGui::Checkbox("Flip Y", &sprite.FlipY);

            ImGui::TreePop();
        }
    }
}