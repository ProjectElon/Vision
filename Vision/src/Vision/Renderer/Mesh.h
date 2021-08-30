#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Renderer/RendererTypes.h"

#include <vector>
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

        VertexLayout Layout;
        VertexBuffer VertexBuffer;
        IndexBuffer  IndexBuffer;
    };
}
