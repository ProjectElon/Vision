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

	private:
		ComponentStateMap m_ComponentState;

    private:
        void DrawComponents(Entity entity, const EntityStorage& storage);
        void DrawComponentSettings(Entity entity, ComponentID componentID, const ComponentInfo& info);
	};
}