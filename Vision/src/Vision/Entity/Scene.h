#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"
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
        std::string Name = "Untitled";
        uint32 EntityCount = 0; //@don't change
        uint32 MaxEntityCount = 100; //@don't change

        std::string PrimaryCameraTag;

        Scene();
        ~Scene();

        void ReConstruct();

        template<typename ... Components>
        Entity CreateEntity(const std::string& tag, const Components ... components)
        {
            if (EntityCount == MaxEntityCount)
            {
                VN_CORE_INFO("Can't create more than the max number of entites");
                return entity::null;
            }

            if (QueryEntity(tag) != entity::null)
            {
                VN_CORE_INFO("Can't create an entity with a taken tag : {0}", tag);
                return entity::null;
            }

            Entity entity = EntityCount + 1;
            EntityCount++;

            m_Tags.insert_or_assign(tag, entity);

            AddComponents(entity, TagComponent { tag }, components...);

            return entity;
        }

        Entity QueryEntity(const std::string& tag);

        void FreeEntity(const std::string& tag);

        void EachEntity(const std::function<void(Entity)>& callbackFn);

        template<typename Component>
        void EachComponent(const std::function<void(Component&)>& callbackFn)
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
        void EachGroup(const std::function<void(Component&, Components&...)>& callbackFn)
        {
            for (Entity entity = 1;
                 entity <= EntityCount;
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
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            ComponentStorage& componentStorage = m_Components[componentID];

            if (!componentStorage.Data)
            {
                componentStorage.SizeInBytes = sizeof(Component);

                componentStorage.Data     = new uint8[MaxEntityCount * sizeof(Component)];
                componentStorage.Entities = new Entity[MaxEntityCount];
            }

            uint8* data = componentStorage.Data;
            Entity* entites = componentStorage.Entities;

            uint32 componentIndex = componentStorage.Count;
            componentStorage.Count++;

            entites[componentIndex] = entity;
            m_Entities[entity].insert_or_assign(componentID, componentIndex);

            uint8* componentPointer = &data[componentIndex * sizeof(Component)];
            memcpy(componentPointer, &component, sizeof(Component));
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
            VN_CORE_ASSERT(entity != entity::null && entity <= EntityCount, "Entity is not valid");

            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            const EntityStorage& entityStorage = m_Entities[entity];

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

            VN_CORE_ASSERT(entity != entity::null && entity <= EntityCount, "Entity is not valid");

            EntityStorage& entityStorage = m_Entities[entity];

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

        inline void* GetComponent(ComponentID componentID, ComponentIndex componentIndex)
        {
            ComponentStorage& componentStorage = m_Components[componentID];
            uint8* data = componentStorage.Data;
            return &data[componentIndex * componentStorage.SizeInBytes];
        }

        inline void* GetComponent(Entity entity, ComponentID componentID)
        {
            EntityStorage& entityStorage = m_Entities[entity];
            ComponentIndex componentIndex = entityStorage[componentID];
            return GetComponent(componentID, componentIndex);
        }

        void RemoveComponent(Entity entity, ComponentID componentID, const std::string& name = "T");

        static void SetActiveScene(Scene* scene);
        inline static Scene& GetActiveScene() { return *s_ActiveScene; }

        friend class SceneSerializer;

    private:
        TagMap m_Tags;

        EntityStorage* m_Entities = nullptr;
        ComponentMap   m_Components;

        static Scene* s_ActiveScene;

#ifdef VN_EDITOR
    public:
        static EditorState EditorState;
        friend class InspectorPanel;
#endif
    };
}