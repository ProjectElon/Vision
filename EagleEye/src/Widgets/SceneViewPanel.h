#pragma once

#include "Vision/Core/Common.h"

#include <glm/glm.hpp>

namespace Vision
{
	struct FrameBuffer;

	struct SceneViewPanel
	{
	public:
		FrameBuffer* FrameBuffer = nullptr;
		uint32 ActiveSceneID = 0;
		int32 GizmoType = -1;

		glm::vec2 ViewportSize = { 0.0f, 0.0f };
		glm::vec2 ViewportBounds[2] = { { 0.0f, 0.0f },
										{ 0.0f, 0.0f } };

		bool IsInteractable = false;
		bool IsViewportResized = false;

		SceneViewPanel() = default;

		void OnImGuiRender();
	};
}