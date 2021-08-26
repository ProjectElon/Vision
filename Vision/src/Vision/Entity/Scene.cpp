#include "pch.hpp"
#include "Vision/Core/Logger.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
    void CreateScene(Scene* scene, uint32 maxEntityCount)
    {
        scene->MaxEntityCount = maxEntityCount;
        scene->Entities = new EntityStorage[maxEntityCount + 1];

        for (auto& [componentID, componentStorage] : scene->Components)
        {
            componentStorage.ReConstruct(maxEntityCount, componentStorage.Alignment);
        }
    }

    void DestroyScene(Scene* scene)
    {
        delete[] scene->Entities;
        scene->Entities = nullptr;
    }

    void Scene::FreeEntity(const std::string& tag)
    {
        Entity entity = QueryEntity(tag);

        VnCoreAssert(entity && entity <= EntityCount);

        Entity lastEntity = EntityCount;

        std::string swappedEntityTag = GetComponent<TagComponent>(lastEntity).Tag.Data;
        std::string removedEntityTag = GetComponent<TagComponent>(entity).Tag.Data;

        Tags.extract(removedEntityTag);

        EntityStorage& storage = Entities[entity];

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
            Tags.insert_or_assign(swappedEntityTag, entity);

            std::swap(Entities[lastEntity], Entities[entity]);

            for (const auto& [componentID, componentIndex] : Entities[entity])
            {
                ComponentStorage& components = Components[componentID];
                components.Entities[componentIndex] = entity;
            }
        }

        VnCoreInfo("Freeing Entity: {0}", tag);
    }

    Entity Scene::QueryEntity(const std::string& tag)
    {
        auto tagIter = Tags.find(tag);

        if (tagIter == Tags.end())
        {
            return 0;
        }

        return tagIter->second;
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

    void Scene::RemoveComponent(Entity entity,
                                ComponentID componentID,
                                const std::string& name)
    {
        VnCoreAssert(entity && entity <= EntityCount);

        EntityStorage& entityStorage = Entities[entity];
        auto entityStorageIter = entityStorage.find(componentID);

        VnCoreAssert(entityStorageIter != entityStorage.end(), "Entity doesn't own Component of Type: " + name);

        const ComponentIndex& currentComponentIndex = entityStorageIter->second;

        ComponentStorage& componentStorage = Components[componentID];

        const uint32& componentSize = componentStorage.SizeInBytes;
        uint8* data = componentStorage.Data;
        Entity* entites = componentStorage.Entities;

        const ComponentIndex lastComponentIndex = componentStorage.Count - 1;
        VnCoreAssert(lastComponentIndex >= 0);

        static uint8 tempStorage[1024];
        VnCoreAssert(componentSize <= 1024);

        if (lastComponentIndex != currentComponentIndex)
        {
            Entity lastComponentEntity = entites[lastComponentIndex];

            std::swap(entites[currentComponentIndex], entites[lastComponentIndex]);

            memcpy(tempStorage, &data[currentComponentIndex * componentSize], componentSize);
            memcpy(&data[currentComponentIndex * componentSize], &data[lastComponentIndex * componentSize], componentSize);
            memcpy(&data[lastComponentIndex * componentSize], tempStorage, componentSize);

            Entities[lastComponentEntity][componentID] = currentComponentIndex;
        }

        entityStorage.extract(entityStorageIter);

        componentStorage.Count--;
    }
}