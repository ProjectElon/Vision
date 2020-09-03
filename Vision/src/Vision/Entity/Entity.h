#pragma once

#include "Vision/Core/Base.h"

namespace Vision
{
	class Scene;
	using EntityHandle = uint32;

	class Entity
	{
	public:
		Entity(EntityHandle handle, Scene* scene);

		template<typename Component>
		inline void AddComponent(const Component&& component)
		{
			m_Scene->AddComponent(m_Handle, component);
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

		template<typename Component, typename ... Components>
		inline auto GetComponents()
		{
			return m_Scene->GetComponents<Component, Components...>(m_Handle);
		}

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

	private:
		EntityHandle m_Handle;
		Scene* m_Scene;

		friend class Scene;
	};
}