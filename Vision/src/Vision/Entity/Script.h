#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
    class Script
    {
	public:
		Entity Entity;
		uint32 Index;

		template<typename Component>
		inline auto& AddComponent(const Component&& component)
		{
			return Entity.AddComponent(component);
		}

		template<typename Component, typename ... Components>
		inline void AddComponents(const Component&& component, const Components&&... components)
		{
			Entity.AddComponents(component, components...);
		}

		template<typename Component>
		inline bool HasComponent()
		{
			return Entity.HasComponent<Component>();
		}

		template<typename Component, typename ... Components>
		inline bool HasComponents()
		{
			return Entity.HasComponents<Component, Components...>();
		}

		template<typename Component>
		inline Component& GetComponent()
		{
			return Entity.GetComponent<Component>();
		}

		template<typename Component, typename ... Components>
		inline auto GetComponents()
		{
			return Entity.GetComponents<Component, Components...>();
		}

		template<typename Component>
		inline Component RemoveComponent()
		{
			return Entity.RemoveComponent<Component>();
		}

		template<typename Component, typename ...Components>
		inline auto RemoveComponents()
		{
			return Entity.RemoveComponents<Component, Components...>();
		}

		template<typename Script>
		inline void AddScript()
		{
			Entity.AddScript<Script>();
		}

		template<typename Script>
		inline void RemoveScript()
		{
			Entity.RemoveScript<Script>();
		}
    };
}