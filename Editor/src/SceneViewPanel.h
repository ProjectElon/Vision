#pragma once

#include <glm/glm.hpp>

namespace Vision
{
	class FrameBuffer;

	class SceneViewPanel
	{
	public:
		SceneViewPanel();

		void SetFrameBuffer(FrameBuffer* SceneFrameBuffer);
		
		void OnImGuiRender();
		
		inline bool IsIntractable() { return m_IsInteractable; }
		inline bool IsViewportResized() { return m_IsViewportResized; }
		inline const glm::vec2& GetViewportSize() { return m_ViewportSize; }

	private:
		bool m_IsInteractable;
		bool m_IsViewportResized;
		bool m_FoucsOnStartUp = true;

		glm::vec2 m_ViewportSize;
			
		FrameBuffer* m_FrameBuffer; 
	};
}