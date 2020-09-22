#include "GameEngineScript.h"
#include "Vision/Platform/Input.h"

namespace Vision
{
	ShowInInspector(GameEngineScript)
	{
		auto& script = component_cast<GameEngineScript>(component);

		bool expanded = ImGui::TreeNodeEx("Game Engine Script", ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding);

		if (expanded)
		{
			static char Buffer[256];
			memset(Buffer, 0, sizeof(Buffer));
			memcpy(Buffer, script.Message.c_str(), script.Message.size());
			
			if (ImGui::InputText("Message", Buffer, sizeof(Buffer)))
			{
				script.Message = Buffer;
			}

			ImGui::TreePop();
		}

		return expanded;
	}

	void GameEngineScript::OnCreate()
	{
	}

	void GameEngineScript::OnDestroy()
	{
	}

	void GameEngineScript::OnUpdate(float deltaTime)
	{
	}
}