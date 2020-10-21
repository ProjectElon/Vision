#include "pch.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
	Scene::Scene(const std::string& name, uint32 maxEntityCount)
		: Name(name)
        , MaxEntityCount(maxEntityCount)
    {
        m_Entites = new EntityStorage[maxEntityCount + 1];
	}

    Scene::~Scene()
    {
        // printing entity data
        VN_CORE_INFO("Scene Format: ");

        VN_CORE_INFO("Scene: {0}", Name);
        VN_CORE_INFO("Entity Count: {0}", EntityCount);
        VN_CORE_INFO("Max Entity Count: {0}", MaxEntityCount);

        VN_CORE_INFO("Entites: ");

        for (uint32 entity = 1;
             entity <= EntityCount;
             ++entity)
        {
            auto& tagComponent = GetComponent<TagComponent>(entity);

            VN_CORE_INFO("\tEntityTag: {0}", tagComponent.Tag);
            VN_CORE_INFO("\tComponentCount: {0}", m_Entites[entity].size());
            VN_CORE_INFO("\tEntityComponents: ");
            VN_CORE_INFO(" ");

            for (const auto& [componentID, componentIndex] : m_Entites[entity])
            {
                const ComponentInfo& info = EditorState.ComponentInspector[componentID];

                VN_CORE_INFO("\t\tName: {0}", info.Name);
                VN_CORE_INFO("\t\tComponentID: {0}", componentID);
                VN_CORE_INFO("\t\tComponentIndex: {0}", componentIndex);
                VN_CORE_INFO(" ");
            }
        }

        VN_CORE_INFO("Components: ");

        for (const auto& [componentID, componentStorage] : m_Components)
        {
            const ComponentInfo& info = EditorState.ComponentInspector[componentID];

            VN_CORE_INFO("\tName: {0}", info.Name);
            VN_CORE_INFO("\tID: {0}", componentID);
            VN_CORE_INFO("\tSizeInBytes: {0}", componentStorage.SizeInBytes);
            VN_CORE_INFO("\tCount: {0}", componentStorage.Count);
            VN_CORE_INFO("\tMemory: {0}", (char*)componentStorage.Data);
            VN_CORE_INFO("\tEntites: ");
            VN_CORE_INFO(" ");
            for (uint32 componentIndex = 0;
                 componentIndex < componentStorage.Count;
                 ++componentIndex)
            {
                Entity entity = componentStorage.Entites[componentIndex];
                auto& tagComponent = GetComponent<TagComponent>(entity);

                VN_CORE_INFO("\t\t Entity: {0}, {1}", entity, tagComponent.Tag);
            }
            VN_CORE_INFO(" ");
        }

        delete[] m_Entites;
    }

    void Scene::FreeEntity(const std::string& tag)
    {
        Entity entity = QueryEntity(tag);
        Entity lastEntity = EntityCount;

        VN_CORE_ASSERT(entity != entity::null && entity <= EntityCount, "Can't Free an invalid Entity");

        if (entity != lastEntity)
        {
            const auto& swappedEntityTag = GetComponent<TagComponent>(lastEntity).Tag;
            m_Tags.insert_or_assign(swappedEntityTag, entity);
        }

        const auto& removedEntityTag = GetComponent<TagComponent>(entity).Tag;

        m_Tags.insert_or_assign(removedEntityTag, entity::null);

        if (entity != lastEntity)
        {
            std::swap(m_Entites[lastEntity], m_Entites[entity]);
        }

        EntityStorage& storage = m_Entites[lastEntity];

        auto entityStorageIter = storage.begin();

        while (entityStorageIter != storage.end())
        {
            ComponentID componentID = entityStorageIter->first;

            entityStorageIter++;

            RemoveComponent(lastEntity, componentID, entity);
        }

        storage.clear();

        EntityCount--;

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

    void Scene::EachEntity(std::function<void(Entity)> callbackFn)
    {
        for (Entity entityHandle = 1;
             entityHandle <= EntityCount;
             ++entityHandle)
        {
            callbackFn(entityHandle);
        }
    }

    void Scene::RemoveComponent(Entity entity, ComponentID componentID, Entity swappedEntity, const std::string& name)
    {
        VN_CORE_ASSERT(entity != entity::null && entity <= EntityCount, "Can't Remove a component of an invalid Entity");

        EntityStorage& entityStorage = m_Entites[entity];
        auto entityStorageIter = entityStorage.find(componentID);

        VN_CORE_ASSERT(entityStorageIter != entityStorage.end(), "Entity doesn't own Component of Type: " + name);

        const ComponentIndex& currentComponentIndex = entityStorageIter->second;

        ComponentStorage& componentStorage = m_Components[componentID];

        const uint32& componentSize = componentStorage.SizeInBytes;
        uint8* data = componentStorage.Data;
        Entity* entites = componentStorage.Entites;

        const ComponentIndex lastComponentIndex = componentStorage.Count - 1;
        VN_CORE_ASSERT(lastComponentIndex >= 0, "lastComponentIndex is less than zero");

        static uint8 tempStorage[1024];
        VN_CORE_ASSERT(componentSize <= 1024, "Component Size is more than 1024 bytes");

        if (lastComponentIndex != currentComponentIndex)
        {
            std::swap(entites[currentComponentIndex], entites[lastComponentIndex]);

            memcpy(tempStorage, &data[currentComponentIndex * componentSize], componentSize);
            memcpy(&data[currentComponentIndex * componentSize], &data[lastComponentIndex * componentSize], componentSize);
            memcpy(&data[lastComponentIndex * componentSize], tempStorage, componentSize);

            Entity currentComponentEntity = entites[currentComponentIndex];
            m_Entites[currentComponentEntity][componentID] = currentComponentIndex;
            
            if (swappedEntity != entity::null)
            {
                entites[currentComponentIndex] = swappedEntity;
            }
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