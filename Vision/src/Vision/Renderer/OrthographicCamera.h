#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/MouseEvent.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	struct OrthographicCamera
	{
		glm::vec3 Position = { 0.0f, 0.0f, 0.0f  };
		float32  MovementSpeed = 1.0f;

		float32 AspectRatio = 1.0f;
        float32 OrthographicSize = 1.0f;

        float32 Near = -1.0f;
        float32 Far  =  1.0f;

        glm::mat4 Projection = glm::mat4(1);
	};

	void InitOrthographicCamera(OrthographicCamera* camera,
								float32 aspectRatio,
								float32 orthographicSize = 1.0f,
								float32 Near = -1.0f,
								float32 Far = 1.0f);

	void ResizeOrthographisCamera(OrthographicCamera* camera,
								  uint32 width,
								  uint32 height);

	void UpdateOrthographicCamera(OrthographicCamera* camera,
								  float32 deltaTime);

	void UpdateOrthographicCameraSize(OrthographicCamera* camera, float32 size);
}