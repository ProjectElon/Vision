#include "GameViewPanel.h"
#include <Vision.h>
#include <imgui.h>

namespace Vision
{
	GameViewPanel::GameViewPanel()
		: m_IsInteractable(false)
		, m_IsViewportResized(false)
		, m_ViewportSize(glm::vec2(0.0f, 0.0f))
		, m_FrameBuffer(nullptr)
	{
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

			Scene& scene = Scene::GetActiveScene();

			scene.EachComponent<OrthographicCameraComponent>([&](auto& camera)
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

			scene.EachComponent<PerspectiveCameraComponent>([&](auto& camera)
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

		ImGui::End();
	}
}