#pragma once

#include <glm/glm.hpp>

namespace Vision
{
	class FrameBuffer;
	class Scene;

	class GameViewPanel
	{
	public:
		void SetContext(Scene* scene);
		void SetFrameBuffer(FrameBuffer* SceneFrameBuffer);
		
		void OnImGuiRender();
		
		inline bool IsIntractable() { return m_IsInteractable; }
		inline bool IsViewportResized() { return m_IsViewportResized; }
		inline const glm::vec2& GetViewportSize() { return m_ViewportSize; }
		
	private:
		Scene* m_Scene = nullptr;

		bool m_IsInteractable = false;
		bool m_IsViewportResized = false;

		glm::vec2 m_ViewportSize = { 0.0f, 0.0f };
			
		FrameBuffer* m_FrameBuffer = nullptr;
	};
}