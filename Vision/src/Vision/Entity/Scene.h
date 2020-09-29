#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"
#include "Vision/Entity/Script.h"
#include "Vision/Entity/ScriptRuntime.h"

#include <map>
#include <unordered_map>

namespace Vision
{
    class Scene
    {
    public:
        Scene(const std::string& name);
        ~Scene();

        const std::string& GetName() const { return m_Name; }

        template<typename ... Components>
        Entity CreateEntity(const std::string& tag, const Components ... components)
        {
            EntityHandle handle = GenerateEntityID();
            AddComponents(handle, TagComponent { tag }, components...);
            Entity entity = { handle, this };
            entity.SetTag(tag);
            return entity;
        }

        Entity GetEntityByTag(const std::string& tag);

        void RunScripts(float deltaTime);

        void FreeEntity(const Entity& entity);

        void EachEntity(std::function<void(Entity)> callbackFn);

        template<typename Component>
        void EachComponent(std::function<void(Component&, Entity)> callbackFn)
        {
            auto componentTypeIter = s_ComponentTypes.find(std::type_index(typeid(Component)));

            if (componentTypeIter == s_ComponentTypes.end())
            {
                return;
            }

            const ComponentID& componentID = componentTypeIter->second;
            const ComponentStorage& components = m_Components[componentID];

            const uint32& componentSize = *((uint32*)&components[0]);
            const uint32 entitySize = sizeof(EntityHandle);
            const uint32 stride = componentSize + entitySize;

            for (uint32 index = sizeof(uint32);
                index < components.size();
                index += stride)
            {
                Component& component = *((Component*)(&components[index]));
                const EntityHandle& handle = *((uint32*)(&components[index + componentSize]));
                callbackFn(component, { handle, this });
            }
        }

        template<typename Component, typename ... Components>
        void EachGroup(std::function<void(Component&, Components&..., Entity)> callbackFn)
        {
            for (const auto& entity : m_Entites)
            {
                const EntityHandle& handle = entity.first;

                if (HasComponents<Component, Components...>(handle))
                {
                    callbackFn(GetComponent<Component>(handle), GetComponent<Components>(handle)..., { handle, this } );
                }
            }
        }

        void SetActiveCamera(Entity entity);
        Entity GetActiveCamera();

        static inline ComponentTypes& GetComponenetTypes() { return s_ComponentTypes; }
        static uint32 GenerateComponentID();
        static uint32 GenerateEntityID();

        static void SetActiveScene(Scene* scene);
        static Scene& GetActiveScene();

    private:
        bool SetTag(EntityHandle handle, const std::string& tag);

		template<typename Component>
        inline Component& AddComponent(EntityHandle entity, const Component component)
        {
            std::type_index typeIndex(typeid(Component));
            auto it = s_ComponentTypes.find(typeIndex);
            
            ComponentID componentID;
            
            if (it != s_ComponentTypes.end())
            {
                VN_CORE_ASSERT(!(HasComponent<Component>(entity)), "Entity already Owns a Component of Type T");
                componentID = (*it).second;
            }
            else
            {
                static_assert(sizeof(Component) <= MAX_COMPONENT_SIZE_IN_BYTES, "Oversized Component");
                componentID = GenerateComponentID();
                
                s_ComponentTypes.emplace(typeIndex, componentID);

                const std::type_info& typeInfo = typeid(Component);

                if constexpr (std::is_base_of<Script, Component>::value)
                {
                    s_Scripts.push_back(std::make_pair(componentID, GetScriptRuntime<Component>));
                }
            }

            ComponentStorage& storage = m_Components[componentID];
            
            if (storage.empty())
            {
                storage.resize(sizeof(uint32));
                uint32& componentSize = *((uint32*)&storage[0]);
                componentSize = sizeof(Component);
            }

            uint32 componentIndex = storage.size();
            storage.resize(componentIndex + (sizeof(Component) + sizeof(EntityHandle)));

            uint8* componentPointer = &storage[componentIndex];
            memcpy(componentPointer, &component, sizeof(Component));
            memcpy(componentPointer + sizeof(Component), &entity, sizeof(EntityHandle));
            
            m_Entites[entity].emplace(componentID, componentIndex);

            Component& componentRef = component_cast<Component>(componentPointer);

            if constexpr (std::is_base_of<Script, Component>::value)
            {
                componentRef.Entity = { entity, this };
            }

#ifdef VN_EDIT
            InspectComponent<Component>();
#endif

            return componentRef;
        }

        template<typename Component>
        inline void AddComponents(EntityHandle entity, const Component component)
        {
            AddComponent(entity, component);
        }

        template<typename Component, typename ... Components>
        inline void AddComponents(EntityHandle entity, const Component component, const Components... components)
        {
            AddComponents(entity, component);
            AddComponents(entity, components...);
        }

        template<typename Component>
        inline bool HasComponent(EntityHandle entity)
        {
            auto componentInfoIter = s_ComponentTypes.find(std::type_index(typeid(Component)));
            
            if (componentInfoIter == s_ComponentTypes.end())
            {
                return false;
            }

            ComponentID componentID = componentInfoIter->second;
            const EntityStorage& entityStorage = m_Entites[entity];
            
            return entityStorage.find(componentID) != entityStorage.end();
        }

        template<typename Component, typename ... Components>
        inline bool HasComponents(EntityHandle entity)
        {
            constexpr uint32 Count = 1 + sizeof...(Components);

            bool componentsStatus[Count] = { HasComponent<Component>(entity), HasComponent<Components>(entity)... };

            for (uint32 index = 0; index < Count; index++)
            {
                if (!componentsStatus[index])
                {
                    return false;
                }
            }

            return true;
        }

        template<typename Component>
        inline Component& GetComponent(EntityHandle entity)
        {
            auto componentTypeIter = s_ComponentTypes.find(std::type_index(typeid(Component)));

            VN_CORE_ASSERT(componentTypeIter != s_ComponentTypes.end(), "Entity doesn't own Component of Type T");

            const ComponentID& componentID = componentTypeIter->second;
            
            const EntityStorage& entityStorage = m_Entites[entity];

            auto componentIter = entityStorage.find(componentID);

            VN_CORE_ASSERT(componentIter != entityStorage.end(), "Entity doesn't own Component of Type T");

            const ComponentIndex& componentIndex = componentIter->second;

            return *((Component*)(&m_Components[componentID][componentIndex]));
        }

        template<typename Component, typename ... Components>
        inline auto GetComponents(EntityHandle entity)
        {
            return std::forward_as_tuple<Component&, Components&...>(GetComponent<Component>(entity), GetComponent<Components>(entity)...);
        }

        inline const EntityStorage& GetEntityStorage(EntityHandle entity)
        {
            VN_CORE_ASSERT(Entity(entity, this).Isvalid(), "Entity is not valid");
            return m_Entites[entity];
        }

        template<typename Component>
        inline Component RemoveComponent(EntityHandle entity)
        {
            auto componentTypeIter = s_ComponentTypes.find(std::type_index(typeid(Component)));

            VN_CORE_ASSERT(componentTypeIter != s_ComponentTypes.end(), "Entity doesn't own Component of Type T");
            
            const ComponentID& componentID = componentTypeIter->second;
            return *(Component*)RemoveComponent(entity, componentID);
        }

        template<typename Component, typename ... Components>
        inline auto RemoveComponents(EntityHandle entity)
        {
            return std::make_tuple<Component, Components...>(RemoveComponent<Component>(entity), RemoveComponent<Components>(entity)...);
        }

    private:
        std::string m_Name;
        std::unordered_map<std::string, Entity> m_Tags;

        std::unordered_map<EntityHandle, EntityStorage> m_Entites;
        std::unordered_map<ComponentID, ComponentStorage> m_Components;
        
        Entity m_ActiveCamera;

        static ComponentTypes s_ComponentTypes;
        static ScriptRuntimeStorage s_Scripts;
        
        static Scene* s_ActiveScene;

        friend class Entity;

    private:
        void* GetComponent(EntityHandle entity, ComponentID componentID, ComponentIndex componentIndex);
        void* RemoveComponent(EntityHandle entity, ComponentID componentID);

#ifdef VN_EDIT
    private:
        template<typename Component>
        void InspectComponent()
        {
            auto& componentTypes = Scene::GetComponenetTypes();

            auto componentTypeIter = componentTypes.find(std::type_index(typeid(Component)));

            ComponentID componentID;

            if (componentTypeIter != componentTypes.end())
            {
                componentID = componentTypeIter->second;
            }
            else
            {
                componentID = Scene::GenerateComponentID();
                componentTypes.emplace(std::type_index(typeid(Component)), componentID);

                if constexpr (std::is_base_of<Script, Component>::value)
                {
                    s_Scripts.push_back(std::make_pair(componentID, GetScriptRuntime<Component>));
                }
            }

            auto componentInspectorIter = s_ComponentInspectors.find(componentID);
            
            if (componentInspectorIter == s_ComponentInspectors.end())
            {
                s_ComponentInspectors.emplace(componentID, ShowInInspectorFn<Component>);
                s_ComponentAdders.emplace(componentID, std::make_pair(typeid(Component).name(), AddComponentInInspectorFn<Component>));
                s_ComponentRemovers.emplace(componentID, RemoveComponentInInspectorFn<Component>);
            }
        }

        using ComponentInspectorMap = std::unordered_map<ComponentID, std::function<uint32(void*)>>;
        using ComponentAdderMap = std::unordered_map<ComponentID, std::pair<std::string, std::function<bool(Entity*)>>>;
        using ComponentRemoverMap = std::unordered_map<ComponentID, std::function<bool(Entity*)>>;

        static ComponentInspectorMap s_ComponentInspectors;
        static ComponentAdderMap     s_ComponentAdders;
        static ComponentRemoverMap   s_ComponentRemovers;
        
    public:
        Entity SelectedEntity;

        static ComponentInspectorMap& GetComponentInspectors() { return s_ComponentInspectors; }
        static ComponentAdderMap& GetComponentAdders() { return s_ComponentAdders; }
        static ComponentRemoverMap& GetComponentRemovers() { return s_ComponentRemovers; }
#endif
    };
}