#include "GameViewPanel.h"
#include <Vision.h>
#include <imgui.h>

namespace Vision
{
	void GameViewPanel::SetContext(Scene* scene)
	{
		m_Scene = scene;
	}

	void GameViewPanel::SetFrameBuffer(FrameBuffer* frameBuffer)
	{
		m_FrameBuffer = frameBuffer;
	}

	void GameViewPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Game Viewport");
		ImGui::PopStyleVar();

		ImVec2 currentViewportSize = ImGui::GetContentRegionAvail();
		uint32 textureID = m_FrameBuffer->GetColorAttachmentRendererId();
		ImGui::Image((void*)(intptr_t)textureID, { m_ViewportSize.x, m_ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

		if (m_ViewportSize != glm::vec2(currentViewportSize.x, currentViewportSize.y) &&
			currentViewportSize.x > 0.0f &&
			currentViewportSize.y > 0.0f)
		{
			m_ViewportSize = { currentViewportSize.x, currentViewportSize.y };

			float aspectRatio = currentViewportSize.x / currentViewportSize.y;

			if (m_Scene)
			{

				m_Scene->EachComponent<OrthographicCameraComponent>([&](auto& camera)
				{
					if (!camera.Static)
					{
						camera.AspectRatio = aspectRatio;
						camera.Projection = glm::ortho(-camera.AspectRatio * camera.Size,
							camera.AspectRatio * camera.Size,
							-camera.Size,
							camera.Size);
					}
				});

				m_Scene->EachComponent<PerspectiveCameraComponent>([&](auto& camera)
				{
					if (!camera.Static)
					{
						camera.AspectRatio = aspectRatio;
						camera.Projection = glm::perspective(camera.FieldOfView,
							camera.AspectRatio,
							camera.Near,
							camera.Far);
					}
				});
			}
		}

		ImGui::End();
	}
}