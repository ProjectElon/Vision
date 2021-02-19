#pragma once

#include "Vision/Core/Common.h"
#include "Vision/IO/Assets.h"

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
        uint32 Alignment   = 0;

        uint8*  Data     = nullptr;
        uint32* Entities = nullptr;

        void ReConstruct(uint32 MaxEntityCount, uint32 alignment);

        ~ComponentStorage();
    };

    #define MaxEntityNameCount 256

    struct TagComponent
    {
        // std::string Tag;
        char Tag[MaxEntityNameCount];
    };

    struct TransformComponent
    {
        glm::vec3 Position = { 0.0f, 0.0f, 1.0f };
        glm::vec3 Rotation = { 0.0f, 0.0f, 0.0f };
        glm::vec3 Scale    = { 1.0f, 1.0f, 1.0f };
    };

    struct OrthographicCameraComponent
    {
        float32 AspectRatio =  1.7778f;
        float32 Size        =  1.0f;

        float32 Near = -1.0f;
        float32 Far  =  1.0f;

        glm::mat4 Projection = glm::mat4(1);

        bool Static = false;
    };

    struct PerspectiveCameraComponent
    {
        float32 AspectRatio = 1.7778f;
        float32 FieldOfView = 45.0f;

        float32 Near = 0.1f;
        float32 Far  = 1000.0f;

        glm::mat4 Projection = glm::mat4(1.0f);

        bool Static = false;
    };

    struct SpriteRendererComponent
    {
        // @(Harlequin): move a default texture to a local directory for now we use wood.png
        uint32 Texture = Assets::RequestAsset("Assets/Textures/wood.png");

        glm::vec4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

        glm::vec2 BottomLeftUV = { 0.0f, 0.0f };
        glm::vec2 TopRightUV   = { 1.0f, 1.0f };
    };
}