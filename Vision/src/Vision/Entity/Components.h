#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Renderer/Texture2D.h"

#include <imgui.h>
#include <glm/gtx/transform.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <typeindex>
#include <tuple>

#define MAX_COMPONENT_SIZE_IN_BYTES 1024
#define ShowInInspector(Component) template<> uint32 ShowInInspectorFn<Component>(void* component)

namespace Vision
{
	using ComponentID = uint32;
    using ComponentIndex = uint32;
    using ComponentTypes = std::unordered_map<std::type_index, ComponentID>;
    using ComponentStorage = std::vector<uint8>;

    template<typename T>
    inline T& component_cast(void* component)
    {
        return *(T*)(component);
    }

    struct TagComponent
    {
        std::string Tag;
    };
    
    struct TransformComponent
    {
        glm::mat4 Transform = glm::mat4(1);
    };

    struct OrthographicCameraComponent
    {
        float AspectRatio = 1.0f;
        float Size = 1.0f;
        float Near = -1.0f;
        float Far = 1.0f;

        glm::mat4 Projection = glm::mat4(1);
        bool Static = false;
    };

    struct PerspectiveCameraComponent
    {
        float AspectRatio = 1.0f;
        float FieldOfView = 45.0f;
        float Near = 0.1f;
        float Far = 1000.0f;

        glm::mat4 Projection = glm::mat4(1);
        bool Static = false;
    };

    struct SpriteComponent
    {
        Ref<Texture2D> Texture;
        glm::vec4      Color = { 1.0f, 1.0f, 1.0f, 1.0f };
        glm::vec2      BottomLeftPoint = { 0.0f, 0.0f };
        glm::vec2      TopRightPoint = { 1.0f, 1.0f };
        bool           FlipX = false;
        bool           FlipY = false;
    };

#ifdef VN_EDIT
           
    class Entity;

    enum ComponentStateMask
    {
        Expaned = VN_BIT(0),
        Removed = VN_BIT(1)
    };

    template<typename T>
    uint32 ShowInInspectorFn(void* component);

    template<typename Component>
    bool AddComponentInInspectorFn(Entity* entity)
    {
        if (!entity->HasComponent<Component>())
        {
            entity->AddComponent<Component>();
            return true;
        }

        return false;
    }

    template<typename Component>
    bool RemoveComponentInInspectorFn(Entity* entity)
    {
        if (entity->HasComponent<Component>())
        {
            entity->RemoveComponent<Component>();
            return true;
        }

        return false;
    }
#endif
}