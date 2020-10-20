#pragma once

#include <Vision.h>

namespace Vision
{
	using ComponentStateMap = std::map<std::pair<Entity, ComponentID>, ComponentState>;

	class InspectorPanel
	{
	public:
		InspectorPanel();

		void OnImGuiRender();

		const ComponentStateMap& GetComponentStateMap() { return m_ComponentState; }

	private:
		ComponentStateMap m_ComponentState;
	};
}