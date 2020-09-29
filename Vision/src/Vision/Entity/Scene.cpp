#include "pch.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
    Scene* Scene::s_ActiveScene = nullptr;
    ComponentTypes Scene::s_ComponentTypes;
    ScriptRuntimeStorage Scene::s_Scripts;
    
	Scene::Scene(const std::string& name)
		: m_Name(name)
	{
	}

    Scene::~Scene()
    {
    }

    bool Scene::SetTag(EntityHandle handle, const std::string& tag)
    {
        auto tagIter = m_Tags.find(tag);
        
        if (tagIter == m_Tags.end())
        {
            auto& tagComponent = GetComponent<TagComponent>(handle);
            m_Tags.erase(tagComponent.Tag);

            tagComponent.Tag = tag;
            Entity entity = { handle, this };
            m_Tags.emplace(tag, entity);

            return true;
        }
        
        VN_CORE_WARN("Tag: {0} Already Taken", tag);
        return false;
    }
    
    Entity Scene::GetEntityByTag(const std::string& tag)
    {
        VN_CORE_ASSERT(m_Tags.find(tag) != m_Tags.end(), "Can't find Entity With Tag: " + tag);
        return m_Tags[tag];
    }

    void Scene::RunScripts(float deltaTime)
    {
        for (auto& [componentID, getScriptRuntimeFn] : s_Scripts)
        {
            const auto& components = m_Components[componentID];
            const uint32& componentSize = *((uint32*)&components[0]);
            uint32 stride = componentSize + sizeof(EntityHandle);

            for (uint32 index = sizeof(uint32);
                 index < components.size();
                 index += stride)
            {
                const void* component = &components[index];
                ScriptRuntime scriptRuntime = getScriptRuntimeFn(component);
                
                scriptRuntime.OnCreate();
                scriptRuntime.OnUpdate(deltaTime);
                scriptRuntime.OnDestroy();
            }
        }
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

    void Scene::EachEntity(std::function<void(Entity)> callbackFn)
    {
        for (const auto& entity : m_Entites)
        {
            const EntityHandle& handle = entity.first;
            callbackFn({ handle, this });
        }
    }

    void Scene::SetActiveCamera(Entity entity)
    {
        VN_CORE_ASSERT(m_ActiveCamera != entity, "Scene is already active.");
        m_ActiveCamera = entity;
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

    Entity Scene::GetActiveCamera()
    {
        return m_ActiveCamera;
    }

    //--- Static Functions ---//

    void Scene::SetActiveScene(Scene* scene)
    {
        VN_CORE_ASSERT(s_ActiveScene != scene, "Scene is already active.");
        s_ActiveScene = scene;
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

    uint32 Scene::GenerateEntityID()
    {
        static uint32 EntityCount = 0;
        return EntityCount++;
    }

#ifdef VN_EDIT
    Scene::ComponentInspectorMap Scene::s_ComponentInspectors;
    Scene::ComponentAdderMap Scene::s_ComponentAdders;
    Scene::ComponentRemoverMap Scene::s_ComponentRemovers;
#endif
}