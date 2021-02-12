#pragma once

#include "Vision/Core/Common.h"

#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Components.h"
#include "Vision/Entity/EditorState.h"
#include "Vision/IO/Assets.h"

#include <unordered_map>

namespace Vision
{
    using TagMap       = std::unordered_map<std::string, Entity>;
    using ComponentMap = std::unordered_map<ComponentID, ComponentStorage>;

    struct Scene
    {
        uint32 EntityCount = 0;
        uint32 MaxEntityCount = 1000;

        TagMap Tags;
        EntityStorage* Entities = nullptr;
        ComponentMap Components;

        std::string PrimaryCameraTag;

#ifdef VN_EDITOR
        static EditorState EditorState;
#endif

        template<typename ... Components>
        Entity CreateEntity(const std::string& tag, const Components ... components)
        {
            if (EntityCount == MaxEntityCount)
            {
                VnCoreInfo("Can't create more than the max number of entites");
                return entity::null;
            }

            if (QueryEntity(tag) != entity::null)
            {
                VnCoreInfo("Can't create an entity with a taken tag : {0}", tag);
                return entity::null;
            }

            Entity entity = EntityCount + 1;
            EntityCount++;

            Tags.insert_or_assign(tag, entity);

            TagComponent tagComponent;
            strcpy(tagComponent.Tag, tag.c_str());

            AddComponents(entity, tagComponent, components...);

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
            VnCoreAssert(componentIter != m_Components.end(), "Entity doesn't own Component of Type: " + std::string(typeInfo.name()));

            ComponentStorage& componentStorage = componentIter->second;
            const uint32 componentSize = componentStorage.SizeInBytes;
            uint8* data = componentStorage.Data;

            for (uint32 componentIndex = 0;
                 componentIndex < componentStorage.Count;
                 componentIndex++)
            {
                Component& component = ComponentCast<Component>(&data[componentIndex * componentSize]);
                callbackFn(component);
            }
        }

        template<typename Component, typename ... Components>
        void EachGroup(const std::function<void(Entity entity, Component&, Components&...)>& callbackFn)
        {
            for (Entity entity = 1;
                 entity <= EntityCount;
                 ++entity)
            {
                if (HasComponents<Component, Components...>(entity))
                {
                    callbackFn(entity, GetComponent<Component>(entity), GetComponent<Components>(entity)...);
                }
            }
        }

		template<typename Component>
        inline void AddComponent(Entity entity, const Component& component = Component())
        {
            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            ComponentStorage& componentStorage = Components[componentID];

            if (componentStorage.Data == nullptr)
            {
                componentStorage.SizeInBytes = sizeof(Component);

                componentStorage.Data     = (uint8*)new Component[MaxEntityCount];
                componentStorage.Entities = new Entity[MaxEntityCount];
            }

            uint32 componentIndex = componentStorage.Count;
            componentStorage.Count++;

            componentStorage.Entities[componentIndex] = entity;
            Entities[entity].insert_or_assign(componentID, componentIndex);

            Component* componentPointer = (Component*)(&componentStorage.Data[componentIndex * sizeof(Component)]);
            memcpy(componentPointer, &component, sizeof(Component));
        }

        template<typename Component, typename ... Components>
        inline void AddComponents(Entity entity, const Component& component, const Components&... components)
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
            VnCoreAssert(entity != entity::null && entity <= EntityCount, "Entity is not valid");

            const std::type_info& typeInfo = typeid(Component);
            const ComponentID& componentID = typeInfo.hash_code();

            const EntityStorage& entityStorage = Entities[entity];

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

            VnCoreAssert(entity != entity::null && entity <= EntityCount, "Entity is not valid");

            EntityStorage& entityStorage = Entities[entity];

            VnCoreAssert(entityStorage.find(componentID) != entityStorage.end(), "Entity doesn't own Component of Type: " + std::string(typeInfo.name()));

            const ComponentIndex& componentIndex = entityStorage[componentID];
            uint8* data = Components[componentID].Data;
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
            ComponentStorage& componentStorage = Components[componentID];
            uint8* data = componentStorage.Data;
            return &data[componentIndex * componentStorage.SizeInBytes];
        }

        inline void* GetComponent(Entity entity, ComponentID componentID)
        {
            EntityStorage& entityStorage = Entities[entity];
            ComponentIndex componentIndex = entityStorage[componentID];
            return GetComponent(componentID, componentIndex);
        }

        void RemoveComponent(Entity entity, ComponentID componentID, const std::string& name = "T");
    };

    void CreateScene(Scene* scene, uint32 maxEntityCount);
    void DestroyScene(Scene* scene);

    AssetLoadingData LoadScene(const std::string& scenepath);
    void UnloadScene(Asset* sceneAsset);
}