#pragma once

#include <Vision.h>

namespace Vision
{
	using ComponentStateMap = std::map<std::pair<EntityHandle, ComponentID>, uint32>;	

	class InspectorPanel
	{
	public:
		InspectorPanel();

		void OnImGuiRender();

		const ComponentStateMap& GetComponentStateMap() { return m_ComponentState; }

	private:
		ComponentStateMap m_ComponentState;
		std::vector<ComponentID> m_ComponentsToRemove;
	};
}