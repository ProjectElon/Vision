#pragma once

#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
    struct Scene;

    struct ComponentState
    {
        bool Open = true;
    };

    using ComponentAddFn         = std::function<void(Scene* scene, Entity)>;
    using ComponentInspectFn     = std::function<void(Scene* scene, void* component)>;
    using ComponentSerializeFn   = std::function<std::string(void* component)>;
    using ComponentDeserializeFn = std::function<void(void* component, const std::string& contents)>;

    struct ComponentInfo
    {
        std::string Name;
        bool        Removable;

        ComponentAddFn         AddFn;
        ComponentInspectFn     InspectFn;
        ComponentSerializeFn   SerializeFn;
        ComponentDeserializeFn DeserializeFn;
    };

    using ComponentMetaMap = std::unordered_map<ComponentID, ComponentInfo>;

    struct EditorState
    {
        std::string      SelectedEntityTag;
        ComponentMetaMap ComponentMeta;

        OrthographicCamera SceneCamera;
    };

    template<typename Component, bool Removable = true>
    static void InspectComponent(const std::string& name, const ComponentInspectFn& inspectFn)
    {
        const std::type_info& typeInfo = typeid(Component);
        const ComponentID& componentID = typeInfo.hash_code();

        ComponentInfo componentInfo;
        componentInfo.Name      = name;
        componentInfo.Removable = Removable;
        componentInfo.InspectFn = inspectFn;
        componentInfo.AddFn     = AddComponentInInspectorFn<Component>;

        Scene::EditorState.ComponentMeta.insert_or_assign(componentID, componentInfo);
    }

    template<typename Component>
    static void SerializeComponent(const ComponentSerializeFn& serializeFn)
    {
        const std::type_info& typeInfo = typeid(Component);
        const ComponentID& componentID = typeInfo.hash_code();

        ComponentInfo& info = Scene::EditorState.ComponentMeta[componentID];
        info.SerializeFn    = serializeFn;
    }

    template<typename Component>
    static void DeserializeComponent(const ComponentDeserializeFn& deserializeFn)
    {
        const std::type_info& typeInfo = typeid(Component);
        const ComponentID& componentID = typeInfo.hash_code();

        ComponentInfo& info = Scene::EditorState.ComponentMeta[componentID];
        info.DeserializeFn  = deserializeFn;
    }

    template<typename Component>
    void AddComponentInInspectorFn(Scene* scene, Entity entity)
    {
        scene->AddComponent<Component>(entity);
    }
}