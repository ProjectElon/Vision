#include "pch.h"
#include "Vision/Renderer/OrthographicCameraController.h"
#include "Vision/Core/Base.h"
#include "Vision/Platform/Input.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, float size, float Near, float Far)
	{
		Camera.AspectRatio = aspectRatio;
		Camera.Size = size;

		Camera.Projection = glm::ortho(-aspectRatio * size,
										  aspectRatio * size,
										 -size,
										  size,
										  Near,
										  Far);
	}

	void OrthographicCameraController::Resize(uint32 width, uint32 height)
	{
		Camera.AspectRatio = (float)width / (float)height;

		Camera.Projection = glm::ortho(-Camera.AspectRatio * Camera.Size,
										  Camera.AspectRatio * Camera.Size,
										 -Camera.Size,
										  Camera.Size);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseWheelScrolledEvent>(VN_BIND_EVENT_FN(OrthographicCameraController::OnMouseWheelScrolled));
	}

	void OrthographicCameraController::OnUpdate(float dt)
	{
		m_CameraMovementSpeed = Camera.Size;

		if (Input::IsKeyDown(Key::W))
		{
			m_CameraPosition.y += m_CameraMovementSpeed * dt;
		}

		if (Input::IsKeyDown(Key::S))
		{
			m_CameraPosition.y -= m_CameraMovementSpeed * dt;
		}
		
		if (Input::IsKeyDown(Key::D))
		{
			m_CameraPosition.x += m_CameraMovementSpeed * dt;
		}

		if (Input::IsKeyDown(Key::A))
		{
			m_CameraPosition.x -= m_CameraMovementSpeed * dt;
		}
	}

	bool OrthographicCameraController::OnMouseWheelScrolled(MouseWheelScrolledEvent& e)
	{
		Camera.Size -= e.GetYOffset() * 0.25f;
		Camera.Size = std::max(Camera.Size, 0.25f);
		Camera.Projection = glm::ortho(-Camera.AspectRatio * Camera.Size,
										Camera.AspectRatio * Camera.Size,
									   -Camera.Size,
										Camera.Size);
		return false;
	}
}