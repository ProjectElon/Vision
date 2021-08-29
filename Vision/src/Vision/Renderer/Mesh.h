#pragma once

#include "Vision/Core/Defines.h"

#include <vector>
#include <iostream>
#include <glm/glm.hpp>

namespace Vision
{
    struct Vertex
    {
        glm::vec3 Position;
        glm::vec2 UV;
        glm::vec3 Normal;
    };

    struct Mesh
    {
        std::vector<Vertex> Vertices;
        std::vector<uint32> Indicies;
    };

    static std::ostream& operator<<(std::ostream& stream, const Vertex& v)
    {
        stream << "p: { " << v.Position.x << ", " << v.Position.y << ", " << v.Position.z
            << " } uv: { " << v.UV.x << ", " << v.UV.y << " } n: { "
            << v.Normal.x << ", " << v.Normal.y << ", " << v.Normal.z << " }";
        return stream;
    }
}
