#include "pch.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
	Scene::Scene()
    {
        ReConstruct();
	}

    Scene::~Scene()
    {
        delete[] m_Entities;
    }

    void Scene::ReConstruct()
    {
        EntityStorage* newStorage = new EntityStorage[MaxEntityCount + 1];

        if (m_Entities)
        {
            memcpy(newStorage + 1, m_Entities + 1, sizeof(EntityStorage) * EntityCount);
            delete[] m_Entities;
        }

        for (auto& [componentID, componentStorage] : m_Components)
        {
            componentStorage.ReConstruct(MaxEntityCount);
        }

        m_Entities = newStorage;
    }

    void Scene::FreeEntity(const std::string& tag)
    {
        Entity entity = QueryEntity(tag);

        VN_CORE_ASSERT(entity != entity::null && entity <= EntityCount, "Can't Free an invalid Entity");

        Entity lastEntity = EntityCount;

        std::string swappedEntityTag = GetComponent<TagComponent>(lastEntity).Tag;
        const std::string& removedEntityTag = GetComponent<TagComponent>(entity).Tag;

        m_Tags.extract(removedEntityTag);

        EntityStorage& storage = m_Entities[entity];

        auto entityStorageIter = storage.begin();

        while (entityStorageIter != storage.end())
        {
            ComponentID componentID = entityStorageIter->first;

            entityStorageIter++;

            RemoveComponent(entity, componentID);
        }

        storage.clear();
        EntityCount--;

        if (entity != lastEntity)
        {
            m_Tags.insert_or_assign(swappedEntityTag, entity);

            std::swap(m_Entities[lastEntity], m_Entities[entity]);

            for (const auto& [componentID, componentIndex] : m_Entities[entity])
            {
                ComponentStorage& components = m_Components[componentID];
                components.Entities[componentIndex] = entity;
            }
        }

        VN_CORE_INFO("Freeing Entity: {0}", tag);
    }

    Entity Scene::QueryEntity(const std::string& tag)
    {
        auto it = m_Tags.find(tag);

        if (it == m_Tags.end())
        {
            return entity::null;
        }

        return it->second;
    }

    void Scene::EachEntity(const std::function<void(Entity)>& callbackFn)
    {
        for (Entity entity = 1;
             entity <= EntityCount;
             ++entity)
        {
            callbackFn(entity);
        }
    }

    void Scene::RemoveComponent(Entity entity, ComponentID componentID, const std::string& name)
    {
        VN_CORE_ASSERT(entity != entity::null && entity <= EntityCount, "Can't Remove a component of an invalid Entity");

        EntityStorage& entityStorage = m_Entities[entity];
        auto entityStorageIter = entityStorage.find(componentID);

        VN_CORE_ASSERT(entityStorageIter != entityStorage.end(), "Entity doesn't own Component of Type: " + name);

        const ComponentIndex& currentComponentIndex = entityStorageIter->second;

        ComponentStorage& componentStorage = m_Components[componentID];

        const uint32& componentSize = componentStorage.SizeInBytes;
        uint8* data = componentStorage.Data;
        Entity* entites = componentStorage.Entities;

        const ComponentIndex lastComponentIndex = componentStorage.Count - 1;
        VN_CORE_ASSERT(lastComponentIndex >= 0, "lastComponentIndex is less than zero");

        static uint8 tempStorage[1024];
        VN_CORE_ASSERT(componentSize <= 1024, "Component Size is more than 1024 bytes");

        if (lastComponentIndex != currentComponentIndex)
        {
            Entity lastComponentEntity = entites[lastComponentIndex];

            std::swap(entites[currentComponentIndex], entites[lastComponentIndex]);

            memcpy(tempStorage, &data[currentComponentIndex * componentSize], componentSize);
            memcpy(&data[currentComponentIndex * componentSize], &data[lastComponentIndex * componentSize], componentSize);
            memcpy(&data[lastComponentIndex * componentSize], tempStorage, componentSize);

            m_Entities[lastComponentEntity][componentID] = currentComponentIndex;
        }

        entityStorage.extract(entityStorageIter);

        componentStorage.Count--;
    }

    void Scene::SetActiveScene(Scene* scene)
    {
        if (s_ActiveScene != scene)
        {
            s_ActiveScene = scene;
        }
        else
        {
            VN_CORE_INFO("Scene is already active.");
        }
    }

    Scene* Scene::s_ActiveScene = nullptr;

#ifdef VN_EDITOR
    EditorState Scene::EditorState;
#endif
}