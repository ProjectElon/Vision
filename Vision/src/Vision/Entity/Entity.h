#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
	class Scene;
	
	using EntityHandle = uint32;
	using EntityStorage = std::unordered_map<ComponentID, ComponentIndex>;

	class Entity
	{
	public:
		Entity();
		Entity(EntityHandle handle, Scene* scene);

		bool SetTag(const std::string& tag);

		bool Isvalid();

		inline const EntityHandle& GetHandle() const { return m_Handle; }

		template<typename Component>
		inline auto& AddComponent(const Component&& component)
		{
			return m_Scene->AddComponent(m_Handle, component);
		}

		template<typename Component, typename ... Components>
		inline void AddComponents(const Component&& component, const Components&&... components)
		{
			m_Scene->AddComponents(m_Handle, component, components...);
		}

		template<typename Component>
		inline bool HasComponent()
		{
			return m_Scene->HasComponent<Component>(m_Handle);
		}

		template<typename Component, typename ... Components>
		inline bool HasComponents()
		{
			return m_Scene->HasComponents<Component, Components...>(m_Handle);
		}

		template<typename Component>
		inline Component& GetComponent()
		{
			return m_Scene->GetComponent<Component>(m_Handle);
		}

		void* GetComponent(ComponentID componentID, ComponentIndex componentIndex);

		template<typename Component, typename ... Components>
		inline auto GetComponents()
		{
			return m_Scene->GetComponents<Component, Components...>(m_Handle);
		}

		const EntityStorage& GetStorage();

		template<typename Component>
		inline Component RemoveComponent()
		{
			return m_Scene->RemoveComponent<Component>(m_Handle);
		}

		template<typename Component, typename ...Components>
		inline auto RemoveComponents()
		{
			return m_Scene->RemoveComponents<Component, Components...>(m_Handle);
		}

		inline bool operator==(const Entity& other)
		{
			return m_Handle == other.m_Handle && m_Scene == other.m_Scene;
		}

		inline bool operator!=(const Entity& other)
		{
			return !(*this == other);
		}

		template<typename Script>
		inline void AddScript()
		{
			m_Scene->AddScript<Script>(m_Handle);
		}

		template<typename Script>
		inline void RemoveScript()
		{
			m_Scene->RemoveScript<Script>(m_Handle);
		}

	private:
		EntityHandle m_Handle;
		Scene* m_Scene;

		friend class Scene;
	};
}