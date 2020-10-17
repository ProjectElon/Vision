#pragma once

#include "Vision/Core/Base.h"

namespace Vision
{
	class Scene;

    class Script
    {
	public:
		Entity Entity;
		Scene* Scene;

		template<typename Component>
		inline void AddComponent(const Component&& component = Component())
		{
			Scene->AddComponent(Entity, component);
		}

		template<typename Component, typename ... Components>
		inline void AddComponents(const Component&& component, const Components&&... components)
		{
			Scene->AddComponents(Entity, component, components...);
		}

		template<typename Component>
		inline bool HasComponent()
		{
			Scene->HasComponent<Component>(Entity);
		}

		template<typename Component, typename ... Components>
		inline bool HasComponents()
		{
			Scene->HasComponents<Component, Components...>(Entity);
		}

		template<typename Component>
		inline Component& GetComponent()
		{
			Scene->GetComponent<Component>(Entity);
		}

		template<typename Component, typename ... Components>
		inline auto GetComponents()
		{
			Scene->GetComponents<Component, Components...>(Entity);
		}

		template<typename Component>
		inline void RemoveComponent()
		{
			Scene->RemoveComponent<Component>(Entity);
		}

		template<typename Component, typename ...Components>
		inline void RemoveComponents()
		{
			Scene->RemoveComponents<Component, Components...>(Entity);
		}
    };
}