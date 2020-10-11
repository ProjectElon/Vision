#pragma once

#include <Vision.h>

namespace Vision
{
	using ComponentStateMap = std::map<std::pair<EntityHandle, ComponentID>, ComponentState>;	

	class InspectorPanel
	{
	public:
		InspectorPanel();

		void OnImGuiRender();

		const ComponentStateMap& GetComponentStateMap() { return m_ComponentState; }

	private:
		ComponentStateMap m_ComponentState;
		std::vector<ComponentID> m_ComponentsToRemove;
		char m_TagBuffer[1024];
	};
}