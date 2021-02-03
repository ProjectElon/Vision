#include "SceneViewPanel.h"

#include "Vision/Renderer/FrameBuffer.h"
#include <imgui.h>

namespace Vision
{
	SceneViewPanel::SceneViewPanel()
		: m_IsInteractable(false)
		, m_IsViewportResized(false)
		, m_ViewportSize(glm::vec2(0.0f, 0.0f))
		, m_FrameBuffer(nullptr)
	{
	}

	void SceneViewPanel::SetFrameBuffer(FrameBuffer* frameBuffer)
	{
		m_FrameBuffer = frameBuffer;
	}

	void SceneViewPanel::OnImGuiRender()
	{
		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Scene Viewport");

		auto viewportOffset = ImGui::GetCursorPos();

		ImGui::PopStyleVar();

		m_IsViewportResized = false;
		m_IsInteractable = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();

		ImVec2 currentViewportSize = ImGui::GetContentRegionAvail();

		uint32 textureID = m_FrameBuffer->ColorAttachments[0];
		ImGui::Image((void*)(intptr_t)textureID, { currentViewportSize.x, currentViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

		auto windowSize = ImGui::GetWindowSize();
		ImVec2 minBound = ImGui::GetWindowPos();

		minBound.x += viewportOffset.x;
		minBound.y += viewportOffset.y;

		ImVec2 maxBound = { minBound.x + windowSize.x, minBound.y + windowSize.y };

		m_ViewportBounds[0] = { minBound.x, minBound.y };
		m_ViewportBounds[1] = { maxBound.x, maxBound.y };

		if (m_ViewportSize != glm::vec2(currentViewportSize.x, currentViewportSize.y) &&
				currentViewportSize.x > 0.0f &&
				currentViewportSize.y > 0.0f)
		{
			m_IsViewportResized = true;
			m_ViewportSize = { currentViewportSize.x, currentViewportSize.y };
			ResizeFrameBuffer(m_FrameBuffer, (uint32)currentViewportSize.x, (uint32)currentViewportSize.y);
		}

		ImGui::End();
	}
}