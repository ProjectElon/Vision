#pragma once

#include <Vision.h> // @(Harlequin): Remove

namespace Vision
{
	using ComponentStateMap = std::map<std::pair<std::string, ComponentID>, ComponentState>;

	struct Scene;

	class InspectorPanel
	{
	public:
		InspectorPanel();

		void OnImGuiRender();

		void SetActiveScene(AssetID scene);

	private:
		AssetID m_ActiveScene = 0;
		ComponentStateMap m_ComponentState;

        void DrawComponents(Scene* scene, Entity entity, const EntityStorage& storage);
	};
}