#pragma once

#include "Vision/Entity/Scene.h"
#include "Vision/Assets/Assets.h"
#include "EditorState.h"

#include <map>

namespace Vision
{
	using ComponentStateMap = std::map<std::pair<std::string, ComponentID>, ComponentState>;

	class InspectorPanel
	{
	public:
		AssetID ActiveSceneID = 0;
		ComponentStateMap ComponentState;

		InspectorPanel();

		void OnImGuiRender();

	private:
        void DrawComponents(Scene* scene, Entity entity, const EntityStorage& storage);
	};
}