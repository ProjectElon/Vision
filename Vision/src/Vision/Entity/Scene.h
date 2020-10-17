#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"
#include "Vision/Entity/Script.h"
#include "Vision/Entity/ScriptRuntime.h"
#include "Vision/Entity/EditorState.h"

#include <map>
#include <unordered_map>

namespace Vision
{
    using TagMap       = std::unordered_map<std::string, Entity>;
    using ComponentMap = std::unordered_map<ComponentID, ComponentStorage>;    
    
    class Scene
    {
    public:
        std::string Name;
        
        std::string ActiveCameraTag;
        
        Scene(const std::string& name, uint32 maxEntityCount);
        ~Scene();

        template<typename ... Components>
        Entity CreateEntity(const std::string& tag, const Components ... components)
        {
            VN_CORE_ASSERT(m_EntityCount <= m_MaxEntityCount, "Can't create more than the max number of entites");
            
            if (QueryEntity(tag) != entity::null)
            {
                VN_CORE_INFO("Can't create an entity with a taken tag : {0}", tag);
                return entity::null;
            }

            Entity entity = m_EntityCount + 1;
            m_EntityCount++;

            m_Tags.insert_or_assign(tag, entity);

            AddComponents(entity, TagComponent { tag }, components...);
            
            return entity;
        }
        
        Entity QueryEntity(const std::string& tag);

        void FreeEntity(const std::string& tag);
        
        void EachEntity(std::function<void(Entity)> callbackFn);

        template<typename Component>
        void EachComponent(std::function<void(Component&)> callbackFn)
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();
           
            auto componentIter = m_Components.find(componentID);
            VN_CORE_ASSERT(componentIter != m_Components.end(), "Entity doesn't own Component of Type: " + std::string(typeInfo.name()));
            
            ComponentStorage& componentStorage = componentIter->second;
            const uint32 componentSize = componentStorage.SizeInBytes;
            uint8* data = componentStorage.Data;

            for (uint32 index = 0;
                 index < componentStorage.Count;
                 index += componentSize)
            {
                Component& component = ComponentCast<Component>(&data[index * componentSize]);
                callbackFn(component);
            }
        }

        template<typename Component, typename ... Components>
        void EachGroup(std::function<void(Component&, Components&...)> callbackFn)
        {
            for (Entity entity = 1;
                 entity <= m_EntityCount;
                 ++entity)
            {
                if (HasComponents<Component, Components...>(entity))
                {
                    callbackFn(GetComponent<Component>(entity), GetComponent<Components>(entity)...);
                }
            }
        }

		template<typename Component>
        inline void AddComponent(Entity entity, const Component& component = Component())
        {
            VN_CORE_ASSERT(entity != entity::null && entity <= m_EntityCount, "Component count can't exceed the max number of entites");
            
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            ComponentStorage& componentStorage = m_Components[componentID];

            if (!componentStorage.Data)
            {
                componentStorage.SizeInBytes = sizeof(Component);
                
                componentStorage.Data    = new uint8[m_MaxEntityCount * sizeof(Component)];
                componentStorage.Entites = new ComponentIndex[m_MaxEntityCount];
            }

            uint8* data = componentStorage.Data;
            Entity* entites = componentStorage.Entites;
            
            size_t componentIndex = componentStorage.Count;
            componentStorage.Count++;

            entites[componentIndex] = entity;
            m_Entites[entity].insert_or_assign(componentID, componentIndex);

            uint8* componentPointer = &data[componentIndex * sizeof(Component)];
            memcpy(componentPointer, &component, sizeof(Component));


            /*
            Todo: Reimplement Scripting idiot ....
            if constexpr (std::is_base_of<Script, Component>::value)
            {
                s_ScriptRuntime.emplace(componentID, GetScriptRuntime<Component>);

                auto component = ComponentCast<Component>(componentPointer);
                
                // @Bug: entity handle can change if we delete an entity
                component.Entity = entity;
                component.Scene  = this;
            }
            */
        }

        template<typename Component, typename ... Components>
        inline void AddComponents(Entity entity, const Component component, const Components... components)
        {
            AddComponent(entity, component);
            
            if constexpr (sizeof...(Components) > 0)
            {
                AddComponents(entity, components...);
            }
        }

        template<typename Component>
        inline bool HasComponent(Entity entity)
        {
            VN_CORE_ASSERT(entity != entity::null && entity <= m_EntityCount, "Entity is not valid");
            
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            const EntityStorage& entityStorage = m_Entites[entity];
            
            return entityStorage.find(componentID) != entityStorage.end();
        }

        template<typename Component, typename ... Components>
        inline bool HasComponents(Entity entity)
        {
            if (!HasComponent<Component>(entity))
            {
                return false;
            }

            if constexpr (sizeof...(Components) > 0)
            {
                return HasComponents<Components...>(entity);
            }

            return true;
        }

        template<typename Component>
        inline Component& GetComponent(Entity entity)
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();
            
            VN_CORE_ASSERT(entity != entity::null && entity <= m_EntityCount, "Entity is not valid");
            
            EntityStorage& entityStorage = m_Entites[entity];

            VN_CORE_ASSERT(entityStorage.find(componentID) != entityStorage.end(), "Entity doesn't own Component of Type: " + std::string(typeInfo.name()));

            const ComponentIndex& componentIndex = entityStorage[componentID];
            uint8* data = m_Components[componentID].Data;
            uint8* componentPointer = &data[componentIndex * sizeof(Component)];
            return ComponentCast<Component>(componentPointer);
        }

        template<typename Component, typename ... Components>
        inline auto GetComponents(Entity entity)
        {
            return std::forward_as_tuple<Component&, Components&...>(GetComponent<Component>(entity), GetComponent<Components>(entity)...);
        }

        template<typename Component>
        inline void RemoveComponent(Entity entity)
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();
            RemoveComponent(entity, componentID, typeInfo.name());
        }

        template<typename Component, typename ... Components>
        inline void RemoveComponents(Entity entity)
        {
            RemoveComponent<Component>(entity);

            if constexpr (sizeof...(Components) > 0)
            {
                RemoveComponents<Components...>(entity);
            }
        }
        
        inline void* GetComponentMemory(ComponentID componentID, ComponentIndex componentIndex)
        {
            ComponentStorage& componentStorage = m_Components[componentID];
            uint8* data = componentStorage.Data;
            return &data[componentIndex * componentStorage.SizeInBytes];
        }

        void RemoveComponent(Entity entity, ComponentID componentID, const std::string& name = "T");

        static void SetActiveScene(Scene* scene);
        inline static Scene& GetActiveScene() { return *s_ActiveScene; }

        uint32 GetEntityCount() const { return m_EntityCount; }
        uint32 GetMaxEntityCount() const { return m_MaxEntityCount; }

    private:
        TagMap m_Tags;

        EntityStorage* m_Entites;
        ComponentMap   m_Components;
        
        uint32 m_EntityCount = 0;
        uint32 m_MaxEntityCount;

        static Scene* s_ActiveScene;
        
#ifdef VN_EDIT
    public:
        static EditorState EditorState;
        friend class InspectorPanel;
#endif
    };
}