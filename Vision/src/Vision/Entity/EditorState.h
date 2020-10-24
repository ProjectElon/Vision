#pragma once

#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
    class Scene;

    struct ComponentState
    {
        bool Open = true;
    };

    using ComponentInspectFn = std::function<void(void*)>;
    using ComponentAddFn     = std::function<void(Entity)>;

    struct ComponentInfo
    {
        std::string        Name;
        bool               Removable;

        ComponentInspectFn InspectFn;
        ComponentAddFn     AddFn;
    };

    using ComponentMetaMap = std::unordered_map<ComponentID, ComponentInfo>;

    struct EditorState
    {
        std::string SelectedEntityTag;
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
    void AddComponentInInspectorFn(Entity entity)
    {
        Scene& scene = Scene::GetActiveScene();
        scene.AddComponent<Component>(entity);
    }
}