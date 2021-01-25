#include "pch.h"
#include "Vision/Entity/Scene.h"
#include "Vision/IO/TextDeserializer.h"

namespace Vision
{
    void CreateScene(Scene* scene, uint32 maxEntityCount)
    {
        scene->MaxEntityCount = maxEntityCount;
        scene->Entities = new EntityStorage[maxEntityCount + 1];

        for (auto& [componentID, componentStorage] : scene->Components)
        {
            componentStorage.ReConstruct(maxEntityCount);
        }
    }

    void DestroyScene(Scene* scene)
    {
        delete[] scene->Entities;
        scene->Entities = nullptr;
    }

    AssetLoadingData LoadScene(const std::string& scenepath)
    {
        AssetLoadingData sceneAsset;

        Scene* scene = new Scene;
        DeserializeScene(scenepath, scene);
        
        sceneAsset.Memory = scene;
        sceneAsset.SizeInBytes = sizeof(Scene);
        sceneAsset.TotalSizeInBytes = sizeof(Scene); // @(Harlequin): not right entites + components

        return sceneAsset;
    }

    void UnloadScene(Asset* sceneAsset)
    {
        Scene* scene = static_cast<Scene*>(sceneAsset->Memory);
        DestroyScene(scene);
        delete scene;
    }

    void Scene::FreeEntity(const std::string& tag)
    {
        Entity entity = QueryEntity(tag);

        VnCoreAssert(entity != entity::null && entity <= EntityCount, "Can't Free an invalid Entity");

        Entity lastEntity = EntityCount;

        std::string swappedEntityTag = GetComponent<TagComponent>(lastEntity).Tag;
        const std::string& removedEntityTag = GetComponent<TagComponent>(entity).Tag;

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
            return entity::null;
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

    void Scene::RemoveComponent(Entity entity, ComponentID componentID, const std::string& name)
    {
        VnCoreAssert(entity != entity::null && entity <= EntityCount, "Can't Remove a component of an invalid Entity");

        EntityStorage& entityStorage = Entities[entity];
        auto entityStorageIter = entityStorage.find(componentID);

        VnCoreAssert(entityStorageIter != entityStorage.end(), "Entity doesn't own Component of Type: " + name);

        const ComponentIndex& currentComponentIndex = entityStorageIter->second;

        ComponentStorage& componentStorage = Components[componentID];

        const uint32& componentSize = componentStorage.SizeInBytes;
        uint8* data = componentStorage.Data;
        Entity* entites = componentStorage.Entities;

        const ComponentIndex lastComponentIndex = componentStorage.Count - 1;
        VnCoreAssert(lastComponentIndex >= 0, "lastComponentIndex is less than zero");

        static uint8 tempStorage[1024];
        VnCoreAssert(componentSize <= 1024, "Component Size is more than 1024 bytes");

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

#ifdef VN_EDITOR
    EditorState Scene::EditorState;
#endif
}