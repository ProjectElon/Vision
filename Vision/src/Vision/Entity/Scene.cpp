#include "pch.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
	Scene::Scene(const std::string& name)
		: Name(name)
	{
	}

    Scene::~Scene()
    {
    }

    void Scene::RemoveComponent(EntityHandle entity, ComponentID componentID, const std::string& name)
    {
        EntityStorage& entityStorage = m_Entites[entity];
        auto componentIter = entityStorage.find(componentID);

        VN_CORE_ASSERT(componentIter != entityStorage.end(), "Entity doesn't own Component of Type: " + name);

        const ComponentIndex& currentComponentIndex = componentIter->second;
        ComponentStorage& componentStorage = m_Components[componentID];
        const uint32& componentSize = componentStorage.SizeInBytes;
        auto& data = componentStorage.Data;
        auto& entites = componentStorage.Entites;

        const ComponentIndex lastComponentIndex = data.size() - componentSize;

        static uint8 tempStorage[1024];
        VN_CORE_ASSERT(componentSize <= 1024, "Component Size is more than 1024 bytes");

        if (lastComponentIndex != currentComponentIndex)
        {
            EntityHandle lastComponentEntity = entites[lastComponentIndex];
            
            uint32 currentIndex = NormalizeComponentIndex(currentComponentIndex, componentSize);
            uint32 lastIndex = NormalizeComponentIndex(lastComponentIndex, componentSize);
            std::swap(entites[currentIndex], entites[lastIndex]);

            memcpy(tempStorage, &data[currentComponentIndex], componentSize);
            memcpy(&data[currentComponentIndex], &data[lastComponentIndex], componentSize);
            memcpy(&data[lastComponentIndex], tempStorage, componentSize);
            
            m_Entites[lastComponentEntity][componentID] = currentComponentIndex;
        }

        entityStorage.erase(componentIter);

        data.resize(data.size() - componentSize);
        entites.resize(entites.size() - 1);
    }

    void Scene::FreeEntity(EntityHandle entity)
    {
        auto entityIter = m_Entites.find(entity);
        
        VN_CORE_ASSERT(entityIter != m_Entites.end(), "Can't Free an invalid Entity");

        const EntityStorage& storage = entityIter->second;

        for (const auto& [componentID, componentIndex] : storage)
        {
            RemoveComponent(entity, componentID);
        }

        m_Entites.erase(entityIter);
    }

    EntityHandle Scene::GetEntityByTag(const std::string& tag)
    {
        VN_CORE_ASSERT(m_Tags.find(tag) != m_Tags.end(), "Can't find an Entity With Tag: " + tag);
        return m_Tags[tag];
    }

    void Scene::EachEntity(std::function<void(EntityHandle)> callbackFn)
    {
        for (const auto& [handle, storage] : m_Entites)
        {
            callbackFn(handle);
        }
    }

    bool Scene::IsEntityValid(EntityHandle entity)
    {
        return m_Entites.find(entity) != m_Entites.end();
    }

    void Scene::SetActiveCamera(const EntityHandle entity)
    {
        VN_CORE_ASSERT(m_ActiveCamera != entity, "Scene is already active.");
        m_ActiveCamera = entity;
    }

    EntityHandle Scene::GetActiveCamera()
    {
        return m_ActiveCamera;
    }

    void Scene::SetActiveScene(Scene* scene)
    {
        VN_CORE_ASSERT(s_ActiveScene != scene, "Scene is already active.");
        s_ActiveScene = scene;
    }

    Scene& Scene::GetActiveScene()
    {
        return *s_ActiveScene;
    }

    Scene* Scene::s_ActiveScene = nullptr;
    ScriptRuntimeMap Scene::s_ScriptRuntime;

#ifdef VN_EDIT

    bool Scene::IsTagAvailable(const std::string& tag)
    {
        auto tagIter = m_Tags.find(tag);
        return tagIter == m_Tags.end();
    }

    void Scene::SetTag(EntityHandle handle, const std::string& tag)
    {
        auto tagIter = m_Tags.find(tag);
        
        auto& tagComponent = GetComponent<TagComponent>(handle);
        m_Tags.erase(tagComponent.Tag);
        
        tagComponent.Tag = tag;
        m_Tags.emplace(tag, handle);
    }

    Scene::ComponentInspectorMap Scene::s_ComponentInspectors;
    Scene::ComponentAdderMap     Scene::s_ComponentAdders;
    
#endif
}