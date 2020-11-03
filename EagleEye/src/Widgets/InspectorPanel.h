#pragma once

#include <Vision.h>

namespace Vision
{
	using ComponentStateMap = std::map<std::pair<std::string, ComponentID>, ComponentState>;

	class InspectorPanel
	{
	public:
		InspectorPanel();

		void OnImGuiRender();

	private:
		ComponentStateMap m_ComponentState;

    private:
        void DrawComponents(Scene& scene, Entity entity, const EntityStorage& storage);
	};
}