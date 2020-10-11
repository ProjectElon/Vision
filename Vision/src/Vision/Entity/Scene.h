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
    using TagMap       = std::unordered_map<std::string, EntityHandle>;
    using EntityMap    = std::unordered_map<EntityHandle, EntityStorage>;
    using ComponentMap = std::unordered_map<ComponentID, ComponentStorage>;    

    class Scene
    {
    public:
        std::string Name;

        Scene(const std::string& name);
        ~Scene();

        template<typename ... Components>
        EntityHandle CreateEntity(const std::string& tag, const Components ... components)
        {
            if (!IsTagAvailable(tag))
            {
                VN_CORE_INFO("Can't create an entity with a taken tag : {0}", tag);
                return 0;
            }

            EntityHandle entity = m_CurrentEntityID++;
            AddComponents(entity, TagComponent { tag }, components...);
            SetTag(entity, tag);
            return entity;
        }

        bool IsEntityValid(EntityHandle entity);

        bool IsTagAvailable(const std::string& tag);
        void SetTag(EntityHandle handle, const std::string& tag);

        void FreeEntity(EntityHandle entity);

        EntityHandle GetEntityByTag(const std::string& tag);

        void EachEntity(std::function<void(EntityHandle)> callbackFn);

        template<typename Component>
        void EachComponent(std::function<void(Component&)> callbackFn)
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();
           
            auto componentIter = m_Components.find(componentID);
            VN_CORE_ASSERT(componentIter != m_Components.end(), "Entity doesn't own Component of Type: " + std::string(typeInfo.name()));
            
            ComponentStorage& componentStorage = componentIter->second;
            const uint32 componentSize = componentStorage.SizeInBytes;
            auto& data = componentStorage.Data;

            for (uint32 index = 0;
                 index < data.size();
                 index += componentSize)
            {
                auto component = ComponentCast<Component>(&data[index]);
                callbackFn(component);
            }
        }

        template<typename Component, typename ... Components>
        void EachGroup(std::function<void(Component&, Components&...)> callbackFn)
        {
            for (const auto& [handle, storage] : m_Entites)
            {
                if (HasComponents<Component, Components...>(handle))
                {
                    callbackFn(GetComponent<Component>(handle), GetComponent<Components>(handle)...);
                }
            }
        }

        void SetActiveCamera(EntityHandle entity);
        EntityHandle GetActiveCamera();

		template<typename Component>
        inline void AddComponent(EntityHandle entity, const Component& component = Component())
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            ComponentStorage& componentStorage = m_Components[componentID];
            auto& data = componentStorage.Data;
            auto& entites = componentStorage.Entites;

            if (data.empty())
            {
                componentStorage.SizeInBytes = sizeof(Component);
            }

            size_t componentIndex = data.size();

            data.resize(componentIndex + sizeof(Component));
            entites.resize(entites.size() + 1);
            
            uint8* componentPointer = &data[componentIndex];
            memcpy(componentPointer, &component, sizeof(Component));
            
            m_Entites[entity].emplace(componentID, componentIndex);

            if constexpr (std::is_base_of<Script, Component>::value)
            {
                s_ScriptRuntime.emplace(componentID, GetScriptRuntime<Component>);

                auto component = ComponentCast<Component>(componentPointer);
                
                // Todo: Rewrite the script struct to support the new changes
                // componentRef.Entity = { entity, this };
            }

            entites[NormalizeComponentIndex(componentIndex, sizeof(Component))] = entity;
        }

        template<typename Component, typename ... Components>
        inline void AddComponents(EntityHandle entity, const Component component, const Components... components)
        {
            AddComponent(entity, component);

            if constexpr (sizeof...(Components) > 0)
            {
                AddComponents(entity, components...);
            }
        }

        template<typename Component>
        inline bool HasComponent(EntityHandle entity)
        {
            auto entityIter = m_Entites.find(entity);
            VN_CORE_ASSERT(entityIter != m_Entites.end(), "Entity is not valid");
            
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            const EntityStorage& entityStorage = entityIter->second;
            
            return entityStorage.find(componentID) != entityStorage.end();
        }

        template<typename Component, typename ... Components>
        inline bool HasComponents(EntityHandle entity)
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
        inline Component& GetComponent(EntityHandle entity)
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();
            
            VN_CORE_ASSERT(m_Entites.find(entity) != m_Entites.end(), "Entity is not valid");
            
            EntityStorage& entityStorage = m_Entites[entity];

            VN_CORE_ASSERT(entityStorage.find(componentID) != entityStorage.end(), "Entity doesn't own Component of Type: " + std::string(typeInfo.name()));

            const ComponentIndex& componentIndex = entityStorage[componentID];
            auto& data = m_Components[componentID].Data;
            uint8* componentPointer = &data[componentIndex];
            return ComponentCast<Component>(componentPointer);
        }

        template<typename Component, typename ... Components>
        inline auto GetComponents(EntityHandle entity)
        {
            return std::forward_as_tuple<Component&, Components&...>(GetComponent<Component>(entity), GetComponent<Components>(entity)...);
        }

        inline EntityStorage& GetEntityStorage(EntityHandle entity)
        {
            VN_CORE_ASSERT(m_Entites.find(entity) != m_Entites.end(), "Entity is not valid");
            return m_Entites[entity];
        }

        template<typename Component>
        inline void RemoveComponent(EntityHandle entity)
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();
            RemoveComponent(entity, componentID, typeInfo.name());
        }

        template<typename Component, typename ... Components>
        inline void RemoveComponents(EntityHandle entity)
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
            auto& data = componentStorage.Data;
            return &data[componentIndex];
        }

        void RemoveComponent(EntityHandle entity, ComponentID componentID, const std::string& name = "T");

        static void SetActiveScene(Scene* scene);
        inline static Scene& GetActiveScene() { return *s_ActiveScene; }

    private:
        TagMap       m_Tags;
        EntityMap    m_Entites;
        ComponentMap m_Components;

        uint32 m_CurrentEntityID = 1;
        EntityHandle m_ActiveCamera;

        static ScriptRuntimeMap s_ScriptRuntime;
        static Scene* s_ActiveScene;

#ifdef VN_EDIT
    public:
        static EditorState EditorState;
#endif
    };
}