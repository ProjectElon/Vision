#pragma once

#include <string>

#include <glm/glm.hpp>

namespace Vision
{
    class ImGuiWidgets
    {
    public:
        static bool DrawBool(const std::string& label, bool& value, float columnWidth = 100.0f);
        static bool DrawFloat(const std::string& label, float& value, float columnWidth = 100.0f);
        static bool DrawFloat2(const std::string& label, glm::vec2& values, float columnWidth = 100.0f, float resetValue = 0.0f);
        static bool DrawFloat3(const std::string& label, glm::vec3& values, float columnWidth = 100.0f, float resetValue = 0.0f);
        static bool DrawColor(const std::string& label, glm::vec4& values);
    };
}