#pragma once

#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

namespace Vision
{
    class Scene;

    struct ComponentState
    {
        bool Open = true;
    };

    using Writer = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    using Reader = rapidjson::Value;

    using ComponentAddFn         = std::function<void(Entity)>;
    using ComponentInspectFn     = std::function<void(void*)>;
    using ComponentSerializeFn   = std::function<void(Writer&, void*)>;
    using ComponentDeserializeFn = std::function<void(const Reader&, void*)>;

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
    void AddComponentInInspectorFn(Entity entity)
    {
        Scene* scene = Scene::GetActiveScene();

        if (scene)
        {
            scene->AddComponent<Component>(entity);
        }
    }
}