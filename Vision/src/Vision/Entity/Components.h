#pragma once

#include "Vision/Core/Base.h"
#include <glm/glm.hpp>

namespace Vision
{
    template<typename T>
    inline T& ComponentCast(void* component)
    {
        return *(T*)(component);
    }

    template<typename T>
    inline T& ComponentCast(uint8* component)
    {
        return *(T*)(component);
    }

    using ComponentID    = uint64;
    using ComponentIndex = uint32;

    struct ComponentStorage
    {
        uint32 SizeInBytes = 0;
        uint32 Count       = 0;

        uint8*  Data    = nullptr;
        uint32* Entities = nullptr;

        void ReConstruct(uint32 MaxEntityCount);

        ~ComponentStorage();
    };

    struct TagComponent
    {
        std::string Tag;
    };

    struct TransformComponent
    {
        glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale    = { 1.0f, 1.0f, 1.0f };
    };

    struct OrthographicCameraComponent
    {
        float AspectRatio =  1.0f;
        float Size        =  1.0f;

        float Near = -1.0f;
        float Far  =  1.0f;

        glm::mat4 Projection = glm::mat4(1);

        bool Static = false;
    };

    struct PerspectiveCameraComponent
    {
        float AspectRatio = 1.0f;
        float FieldOfView = 45.0f;

        float Near = 0.1f;
        float Far  = 1000.0f;

        glm::mat4 Projection = glm::mat4(1);

        bool Static = false;
    };

    class Texture2D;

    struct SpriteRendererComponent
    {
        Texture2D* Texture = nullptr;

        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

        glm::vec2 BottomLeftUV = { 0.0f, 0.0f };
        glm::vec2 TopRightUV   = { 1.0f, 1.0f };

        bool FlipX = false;
        bool FlipY = false;
    };
}