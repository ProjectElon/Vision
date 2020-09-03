#include "pch.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
	Scene::Scene()
		: m_EntityIDCounter(0)
		, m_ComponentIDCounter(0)
	{
	}

    Scene::~Scene()
    {
    }

    void Scene::FreeEntity(const Entity& entity)
    {
        auto entityIter = m_Entites.find(entity.m_Handle);
        VN_CORE_ASSERT(entityIter != m_Entites.end(), "Ent");

        const EntityStorage& storage = entityIter->second;

        for (const auto& component : storage)
        {
            const ComponentID& componentID = component.first;
            RemoveComponent(entity.m_Handle, componentID);
        }

        m_Entites.erase(entityIter);
    }

    void Scene::for_each(void(*CallbackFn)(Entity))
    {
        for (const auto& entity : m_Entites)
        {
            const EntityHandle& handle = entity.first;
            CallbackFn({ handle, this });
        }
    }

    void* Scene::RemoveComponent(EntityHandle entity, ComponentID componentID)
    {
        static uint8 TempStorage[MAX_COMPONENT_SIZE];

        EntityStorage& entityStorage = m_Entites[entity];
        auto componentIter = entityStorage.find(componentID);

        VN_CORE_ASSERT(componentIter != entityStorage.end(), "Entity doesn't own Component");

        const ComponentIndex& currentComponentIndex = componentIter->second;
        ComponentStorage& componentStorage = m_Components[componentID];
        uint32& componentSize = *((uint32*)&componentStorage[0]);

        const ComponentIndex lastComponentIndex = componentStorage.size() - (componentSize + sizeof(EntityHandle));

        memcpy(TempStorage, &componentStorage[currentComponentIndex], componentSize + sizeof(EntityHandle));

        if (lastComponentIndex != currentComponentIndex)
        {
            EntityHandle lastComponentEntity = *(EntityHandle*)(&componentStorage[lastComponentIndex + componentSize]);
            
            memcpy(&componentStorage[currentComponentIndex], &componentStorage[lastComponentIndex], componentSize + sizeof(EntityHandle));
            memcpy(&componentStorage[lastComponentIndex], TempStorage, componentSize + sizeof(EntityHandle));

            m_Entites[lastComponentEntity][componentID] = currentComponentIndex;
        }

        entityStorage.erase(componentIter);
        componentStorage.resize(componentStorage.size() - (componentSize + sizeof(EntityHandle)));

        return TempStorage;
    }
}