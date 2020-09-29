#include "GameManagerScript.h"
#include "Vision/Platform/Input.h"

namespace Vision
{
	ShowInInspector(GameManagerScript)
	{
		auto& script = component_cast<GameManagerScript>(component);

		bool expanded = ImGui::TreeNodeEx("Game Manager Script", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding);

		uint32 flags = 0;

		if (ImGui::Button("-"))
		{
			flags |= ComponentStateMask::Removed;
			const std::type_info& typeInfo = typeid(SpriteComponent);
			VN_CORE_INFO("Removing Component: {0}", typeInfo.name());
		}

		if (expanded)
		{
			flags |= ComponentStateMask::Expaned;
			ImGui::InputFloat("Speed", &script.Speed);
			ImGui::TreePop();
		}

		return flags;
	}

	void GameManagerScript::OnCreate()
	{
		auto& transform = GetComponent<TransformComponent>();
		m_Position = &transform.Transform[3];
	}

	void GameManagerScript::OnDestroy()
	{
	}

	void GameManagerScript::OnUpdate(float deltaTime)
	{
		float movementSpeed = 0.0f;

		if (Input::IsKeyDown(VN_KEY_RIGHT))
		{
			movementSpeed += Speed;
		}

		if (Input::IsKeyDown(VN_KEY_LEFT))
		{
			movementSpeed -= Speed;
		}

		*m_Position += glm::vec4(movementSpeed, 0.0f, 0.0f, 0.0f) * deltaTime;
	}
}