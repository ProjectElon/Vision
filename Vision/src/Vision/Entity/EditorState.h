#pragma once

#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
    class Scene;

    struct ComponentState
    {
        bool Expanded = true;
        bool PendingRemove = false;
    };

    using ComponentInspectFn = std::function<void(void*)>;
    using ComponentAddFn = std::function<void(Entity)>;

    struct ComponentInfo
    {
        std::string        Name;
        bool               Removable;
        ComponentInspectFn InspectFn;
        ComponentAddFn     AddFn;
    };

    using ComponentInfoMap = std::unordered_map<ComponentID, ComponentInfo>;

    struct EditorState
    {
        std::string SeleteEntityTag;
        ComponentInfoMap ComponentInspector;
    };

    template<typename Component, bool Removable = true>
    static void InspectComponent(const std::string& name, const ComponentInspectFn& inspectFn)
    {
        const std::type_info& typeInfo = typeid(Component);
        const ComponentID& componentID = typeInfo.hash_code();

        auto& editorState = Scene::EditorState;

        ComponentInfo componentInfo;
        componentInfo.Name      = name;
        componentInfo.Removable = Removable;
        componentInfo.InspectFn = inspectFn;
        componentInfo.AddFn     = AddComponentInInspectorFn<Component>;

        editorState.ComponentInspector.insert_or_assign(componentID, componentInfo);
    }

    template<typename Component>
    void AddComponentInInspectorFn(uint32 entity)
    {
        Scene& scene = Scene::GetActiveScene();
        scene.AddComponent<Component>(entity);
    }
}