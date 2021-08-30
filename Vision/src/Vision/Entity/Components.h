#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Assets/Assets.h"
#include "Vision/Core/String.h"

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

    introspect_struct("math", true)
    struct Vector2
    {
        float32 x = 0.0f;
        float32 y = 0.0f;

        ignore_methods()

        Vector2() = default;

        Vector2(float32 _x, float32 _y)
            : x(_x)
            , y(_y)
        {
        }

        Vector2(const glm::vec2& v)
        {
            x = v.x;
            y = v.y;
        }

        Vector2& operator=(const glm::vec2& v)
        {
            x = v.x;
            y = v.y;
            return *this;
        }

        operator glm::vec2()
        {
            return glm::vec2(x, y);
        }
    };

    introspect_struct("math", true)
    struct Vector3
    {
        float32 x = 0.0f;
        float32 y = 0.0f;
        float32 z = 0.0f;

        ignore_methods()

        Vector3() = default;

        Vector3(float32 _x, float32 _y, float32 _z)
            : x(_x)
            , y(_y)
            , z(_z)
        {
        }

        Vector3(const glm::vec3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
        }

        Vector3& operator=(const glm::vec3& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            return *this;
        }

        operator glm::vec3()
        {
            return glm::vec3(x, y, z);
        }
    };

    introspect_struct("math", true)
    struct Vector4
    {
        float32 x = 0.0f;
        float32 y = 0.0f;
        float32 z = 0.0f;
        float32 w = 0.0f;

        ignore_methods()

        Vector4() = default;

        Vector4(float32 _x, float32 _y, float32 _z, float32 _w)
            : x(_x)
            , y(_y)
            , z(_z)
            , w(_w)
        {
        }

        Vector4(const glm::vec4& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
        }

        Vector4& operator=(const glm::vec4& v)
        {
            x = v.x;
            y = v.y;
            z = v.z;
            w = v.w;
            return *this;
        }

        operator glm::vec4()
        {
            return glm::vec4(x, y, z, w);
        }
    };

    introspect_struct("math", true)
    struct Matrix4
    {
        float32 elements[4 * 4] = { 0.0f };

        ignore_methods()

        Matrix4() = default;

        Matrix4(const glm::mat4& mat)
        {
            for (int i = 0; i < 16; i++)
            {
                int32 row = i / 4;
                int32 col = i % 4;
                elements[i] = mat[row][col];
            }
        }

        Matrix4& operator=(const glm::mat4& mat)
        {
            for (int i = 0; i < 16; i++)
            {
                int32 row = i / 4;
                int32 col = i % 4;
                elements[i] = mat[row][col];
            }

            return *this;
        }

        operator glm::mat4()
        {
            glm::mat4 m;

            for (int i = 0; i < 16; i++)
            {
                int32 row = i / 4;
                int32 col = i % 4;

                m[row][col] = elements[i];
            }

            return m;
        }
    };

    #define MaxEntityNameCount 256

    introspect_struct("component", true)
    struct TagComponent
    {
        String Tag;
    };

    introspect_struct("component", true)
    struct TransformComponent
    {
        Vector3 Position = { 0.0f, 0.0f, 1.0f };
        Vector3 Rotation = { 0.0f, 0.0f, 0.0f };
        Vector3 Scale    = { 1.0f, 1.0f, 1.0f };
    };

    introspect_struct("component", true)
    struct OrthographicCameraComponent
    {
        float32 AspectRatio =  1.7778f;
        float32 Size        =  1.0f;

        float32 Near = -1.0f;
        float32 Far  =  1.0f;

        Matrix4 Projection = glm::mat4(1);

        bool Static = false;
    };

    introspect_struct("component", true)
    struct PerspectiveCameraComponent
    {
        float32 AspectRatio = 1.7778f;
        float32 FieldOfView = 45.0f;

        float32 Near = 0.1f;
        float32 Far  = 1000.0f;

        Matrix4 Projection = glm::mat4(1.0f);

        bool Static = false;
    };

    introspect_struct("component", true)
    struct SpriteRendererComponent
    {
        // @(Harlequin): move a default texture to a local directory for now we use wood.png
        uint32 Texture = Assets::RequestAsset("Assets/Textures/wood.png");

        Vector4 Color = { 1.0f, 1.0f, 1.0f, 1.0f };

        Vector2 BottomLeftUV = { 0.0f, 0.0f };
        Vector2 TopRightUV = { 1.0f, 1.0f };
    };

    introspect_struct("component", true)
    struct MeshRendererComponent
    {
        uint32 MeshAssetID = Assets::RequestAsset("Assets/Meshes/cube.obj");
        // Todo(Harlequin): Material System
        uint32 TextureAssetID = Assets::RequestAsset("Assets/Textures/wood.png");
        uint32 ShaderAssetID = 0;
    };
}