#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Platform/Input.h"

namespace Vision
{
	void InitOrthographicCamera(OrthographicCamera* camera,
								float32 aspectRatio,
								float32 orthographicSize,
								float32 Near,
								float32 Far)
	{
		camera->AspectRatio = aspectRatio;
		camera->OrthographicSize = orthographicSize;
		camera->MovementSpeed = orthographicSize;
		camera->Projection = glm::ortho(-aspectRatio * orthographicSize,
										 aspectRatio * orthographicSize,
										-orthographicSize,
										 orthographicSize,
										 Near,
										 Far);
	}

	void ResizeOrthographisCamera(OrthographicCamera* camera,
								  uint32 width,
								  uint32 height)
	{
		camera->AspectRatio = (float32)width / (float32)height;

		camera->Projection = glm::ortho(-camera->AspectRatio * camera->OrthographicSize,
										 camera->AspectRatio * camera->OrthographicSize,
										-camera->OrthographicSize,
										 camera->OrthographicSize);
	}

	void UpdateOrthographicCamera(OrthographicCamera* camera,
								  float32 deltaTime)
	{
		if (Input::IsKeyDown(Key::W))
		{
			camera->Position.y += camera->MovementSpeed * deltaTime;
		}

		if (Input::IsKeyDown(Key::S))
		{
			camera->Position.y -= camera->MovementSpeed * deltaTime;
		}

		if (Input::IsKeyDown(Key::D))
		{
			camera->Position.x += camera->MovementSpeed * deltaTime;
		}

		if (Input::IsKeyDown(Key::A))
		{
			camera->Position.x -= camera->MovementSpeed * deltaTime;
		}
	}


	void UpdateOrthographicCameraSize(OrthographicCamera* camera, float32 size)
	{
		camera->OrthographicSize += size * 0.25f;
		camera->OrthographicSize = std::max(camera->OrthographicSize, 0.25f);
		camera->MovementSpeed = camera->OrthographicSize;
		camera->Projection = glm::ortho(-camera->AspectRatio * camera->OrthographicSize,
										 camera->AspectRatio * camera->OrthographicSize,
									    -camera->OrthographicSize,
									 	 camera->OrthographicSize);
	}
}