#include "pch.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
    Scene* Scene::s_ActiveScene = nullptr;
    ComponentTypes Scene::s_ComponentTypes;

	Scene::Scene(const std::string& name)
		: m_Name(name)
        , m_EntityIDCounter(0)
	{
	}

    Scene::~Scene()
    {
    }

    void Scene::Load()
    {
    }

    void Scene::Save()
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

    void Scene::Each(std::function<void(Entity)> CallbackFn)
    {
        for (const auto& entity : m_Entites)
        {
            const EntityHandle& handle = entity.first;
            CallbackFn({ handle, this });
        }
    }

    void* Scene::GetComponent(EntityHandle entity, ComponentID componentID, ComponentIndex componentIndex)
    {
        return &m_Components[componentID][componentIndex];
    }

    void* Scene::RemoveComponent(EntityHandle entity, ComponentID componentID)
    {
        static uint8 TempStorage[MAX_COMPONENT_SIZE_IN_BYTES];

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

    void Scene::SetActiveCamera(Entity entity)
    {
        VN_CORE_ASSERT(m_ActiveCamera != entity, "Scene is already active.");
        m_ActiveCamera = entity;
    }

    Entity Scene::GetActiveCamera()
    {
        return m_ActiveCamera;
    }

    //--- Static Functions ---//

    void Scene::SetActiveScene(Scene* scene)
    {
        VN_CORE_ASSERT(s_ActiveScene != scene, "Scene is already active.");
        s_ActiveScene->Save();
        s_ActiveScene = scene;
        s_ActiveScene->Load();
    }

    Scene& Scene::GetActiveScene()
    {
        return *s_ActiveScene;
    }

    uint32 Scene::GenerateComponentID()
    {
        static uint32 ComponentCount = 0;
        return ComponentCount++;
    }
}