#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Entity.h"
#include "Vision/Entity/Component.h"
#include <typeindex>
#include <type_traits>
#include <tuple>
#include <functional>
#include <bitset>

namespace Vision
{
    class Scene
    {
    public:
        Scene();
        ~Scene();

        template<typename ... Components>
        Entity CreateEntity(const std::string& tag, const Components ... components)
        {
            EntityHandle entity = m_EntityIDCounter++;
            AddComponents(entity, TagComponent { tag }, components...);
            return { entity, this };
        }

        void FreeEntity(const Entity& entity);

        template<typename Component>
        void for_each(void(*CallbackFn)(Component&, Entity))
        {
            auto componentTypeIter = m_ComponentTypes.find(std::type_index(typeid(Component)));

            VN_CORE_ASSERT(componentTypeIter != m_ComponentTypes.end(), "Entity doesn't own Component of Type T");

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
                Entity entity(*((uint32*)(&components[index + componentSize])), this);
                CallbackFn(component, entity);
            }
        }

        template<typename Component, typename ... Components>
        void for_each(void(*CallbackFn)(const std::tuple<Component&, Components&...>&, Entity))
        {
            for (const auto& entity : m_Entites)
            {
                const EntityHandle& handle = entity.first;

                if (HasComponents<Component, Components...>(handle))
                {
                    const auto components = GetComponents<Component, Components...>(handle);
                    CallbackFn(components, { handle, this });
                }
            }
        }

        void for_each(void(*CallbackFn)(Entity));

    private:
		template<typename Component>
        inline void AddComponent(EntityHandle entity, const Component component)
        {
            auto it = m_ComponentTypes.find(std::type_index(typeid(Component)));
            ComponentID componentID;
            
            if (it != m_ComponentTypes.end())
            {
                VN_CORE_ASSERT(!(HasComponent<Component>(entity)), "Entity already Owns a Component of Type T");
                componentID = (*it).second;
            }
            else
            {
                VN_CORE_ASSERT(sizeof(Component) <= MAX_COMPONENT_SIZE, "Oversized Component");
                componentID = m_ComponentIDCounter++;
                m_ComponentTypes.emplace(std::type_index(typeid(Component)), componentID);
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
            auto componentInfoIter = m_ComponentTypes.find(std::type_index(typeid(Component)));
            
            if (componentInfoIter == m_ComponentTypes.end())
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

            static bool componentsStatus[Count] = { HasComponent<Component>(entity), HasComponent<Components>(entity)... };

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
            auto componentTypeIter = m_ComponentTypes.find(std::type_index(typeid(Component)));

            VN_CORE_ASSERT(componentTypeIter != m_ComponentTypes.end(), "Entity doesn't own Component of Type T");

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

        template<typename Component>
        inline Component RemoveComponent(EntityHandle entity)
        {
            auto componentTypeIter = m_ComponentTypes.find(std::type_index(typeid(Component)));

            VN_CORE_ASSERT(componentTypeIter != m_ComponentTypes.end(), "Entity doesn't own Component of Type T");
            
            const ComponentID& componentID = componentTypeIter->second;
            return *(Component*)RemoveComponent(entity, componentID);
        }

        template<typename Component, typename ... Components>
        inline auto RemoveComponents(EntityHandle entity)
        {
            return std::make_tuple<Component, Components...>(RemoveComponent<Component>(entity), RemoveComponent<Components>(entity)...);
        }

        void* RemoveComponent(EntityHandle entity, ComponentID componentID);
		
    private:
        using ComponentStorage = std::vector<uint8>;
        using EntityStorage = std::unordered_map<ComponentID, ComponentIndex>;

        uint32 m_EntityIDCounter = 0;
        std::unordered_map<EntityHandle, EntityStorage> m_Entites;
        
        uint32 m_ComponentIDCounter = 0;
        std::unordered_map<std::type_index, ComponentID> m_ComponentTypes;
        std::unordered_map<ComponentID, ComponentStorage> m_Components;

        friend class Entity;
    };
}