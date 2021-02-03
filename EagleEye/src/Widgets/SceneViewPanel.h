#pragma once

#include <glm/glm.hpp>

namespace Vision
{
	struct FrameBuffer;

	class SceneViewPanel
	{
	public:
		SceneViewPanel();

		void SetFrameBuffer(FrameBuffer* SceneFrameBuffer);

		void OnImGuiRender();

		inline bool IsIntractable() { return m_IsInteractable; }
		inline bool IsViewportResized() { return m_IsViewportResized; }

		inline const glm::vec2& GetViewportSize() { return m_ViewportSize; }
		inline const glm::vec2* GetViewportBounds() { return m_ViewportBounds; }

	private:
		bool m_IsInteractable;
		bool m_IsViewportResized;

		glm::vec2 m_ViewportSize;
		glm::vec2 m_ViewportBounds[2];

		FrameBuffer* m_FrameBuffer;
	};
}