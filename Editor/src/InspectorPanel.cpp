#include "InspectorPanel.h"

namespace Vision
{
	InspectorPanel::InspectorPanel()
	{
		m_ComponentsToRemove.reserve(10);
	}

	void InspectorPanel::OnImGuiRender()
	{
		Scene& scene = Scene::GetActiveScene();

		ImGui::Begin("Inspector");
		
		// Note SeletedEntity can be invalid if we deleted an entity
		if (scene.SelectedEntity)
		{
			const auto& storage = scene.GetEntityStorage(scene.SelectedEntity);
			auto& componentInspectors = Scene::GetComponentInspectors();

			for (auto& [componentID, componentIndex] : storage)
			{
				void* component = scene.GetComponentMemory(componentID, componentIndex);

				auto componentStateIter = m_ComponentState.find({ scene.SelectedEntity, componentID });
				
				if (componentStateIter == m_ComponentState.end())
				{
					m_ComponentState.emplace(std::make_pair(scene.SelectedEntity, componentID), ComponentStateMask::Expaned);
				}

				uint32& flags = m_ComponentState[{ scene.SelectedEntity, componentID }];

				ImGui::SetNextTreeNodeOpen(flags & ComponentStateMask::Expaned);
				auto& inspectComponentFn = componentInspectors[componentID];
				flags = inspectComponentFn(component);

				if (flags & ComponentStateMask::Removed)
				{
					m_ComponentsToRemove.emplace_back(componentID);
				}
			}

			if (!m_ComponentsToRemove.empty())
			{
				for (auto& componentID : m_ComponentsToRemove)
				{
					scene.RemoveComponent(scene.SelectedEntity, componentID);
				}

				m_ComponentsToRemove.resize(0);
			}
			
			if (ImGui::Button("+Add Components"))
			{
				ImGui::OpenPopup("Components");	
			}

			if (ImGui::BeginPopup("Components"))
			{
				auto& componentAdders = Scene::GetComponentAdders();

				for (const auto& [componentID, componentPair] : componentAdders)
				{
					const std::string& componentName = componentPair.first;
					const auto& componentAdderFn = componentPair.second;
					
					if (storage.find(componentID) != storage.end())
					{
						continue;
					}

					if (ImGui::Selectable(componentName.c_str()))
					{
						componentAdderFn(scene.SelectedEntity);
						VN_CORE_INFO("\tAdded Component: {0}", componentName);
					}
				}

				ImGui::EndPopup();
			}
		}

		ImGui::End();
	}
}