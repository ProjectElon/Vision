#include "pch.hpp"

#include "Vision\ImGui\ImGuiWidgets.h"

#include <imgui.h>
#include <imgui_internal.h>

namespace Vision
{
    bool ImGuiWidgets::DrawBool(const std::string& label, bool& value, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        changed = ImGui::Checkbox("##X", &value);

        ImGui::Columns(1);

        ImGui::PopID();

        return changed;
    }

    bool ImGuiWidgets::DrawFloat(const std::string& label, float& value, float columnWidth)
    {
        bool changed = false;

        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);
        ImGui::Text(label.c_str());

        ImGui::NextColumn();
        changed = ImGui::DragFloat("##X", &value, 0.1f);

        ImGui::Columns(1);

        ImGui::PopID();

        return changed;
    }

    bool ImGuiWidgets::DrawFloat2(const std::string& label, glm::vec2& values, float columnWidth, float resetValue)
    {
        bool changed = false;
        ImGui::PushID(label.c_str());

        ImGui::Columns(2);
        ImGui::SetColumnWidth(0, columnWidth);

        ImGui::Text(label.c_str());
        ImGui::NextColumn();

        ImGui::PushMultiItemsWidths(2, ImGui::CalcItemWidth());
        ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2{ 0, 0 });

        float lineHeight = GImGui->Font->FontSize + GImGui->Style.FramePadding.y * 2.0f;
        ImVec2 buttonSize = { lineHeight + 3.0f, lineHeight };

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.9f, 0.2f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.8f, 0.1f, 0.15f, 1.0f });

        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushFont(io.Fonts->Fonts[1]);

        if (ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
        }

        ImGui::PopFont();

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

        ImGui::PushFont(io.Fonts->Fonts[1]);

        if (ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
        }

        ImGui::PopFont();

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();
        ImGui::Columns(1);

        ImGui::PopID();

        return changed;
    }

    bool ImGuiWidgets::DrawFloat3(const std::string& label, glm::vec3& values, float columnWidth, float resetValue)
    {
        bool changed = false;
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

        ImGuiIO& io = ImGui::GetIO();

        ImGui::PushFont(io.Fonts->Fonts[1]);

        if (ImGui::Button("X", buttonSize))
        {
            values.x = resetValue;
        }

        ImGui::PopFont();

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##X", &values.x, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.3f, 0.8f, 0.3f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.2f, 0.7f, 0.2f, 1.0f });

        ImGui::PushFont(io.Fonts->Fonts[1]);

        if (ImGui::Button("Y", buttonSize))
        {
            values.y = resetValue;
        }

        ImGui::PopFont();

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Y", &values.y, 0.1f);
        ImGui::PopItemWidth();
        ImGui::SameLine();

        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4{ 0.2f, 0.35f, 0.9f, 1.0f });
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4{ 0.1f, 0.25f, 0.8f, 1.0f });

        ImGui::PushFont(io.Fonts->Fonts[1]);

        if (ImGui::Button("Z", buttonSize))
        {
            values.z = resetValue;
        }

        ImGui::PopFont();

        ImGui::PopStyleColor(3);

        ImGui::SameLine();
        changed |= ImGui::DragFloat("##Z", &values.z, 0.1f);
        ImGui::PopItemWidth();

        ImGui::PopStyleVar();

        ImGui::Columns(1);

        ImGui::PopID();

        return changed;
    }

    bool ImGuiWidgets::DrawColor(const std::string& label, glm::vec4& values)
    {
        bool changed = false;
        ImGui::PushID(label.c_str());

        ImGui::Text("Color");
        ImGui::SameLine();

        changed |= ImGui::ColorEdit4("##Color", &values.r);

        ImGui::PopID();

        return changed;
    }
}