#include "pch.h"
#include "Vision/Renderer/OrthographicCameraController.h"
#include "Vision/Core/Base.h"
#include "Vision/Platform/Input.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, float size, float Near, float Far)
	{
		m_Camera.AspectRatio = aspectRatio;
		m_Camera.Size = size;

		m_Camera.Projection = glm::ortho(-aspectRatio * size,
										  aspectRatio * size,
										 -size,
										  size,
										  Near,
										  Far);
	}

	void OrthographicCameraController::Resize(uint32 width, uint32 height)
	{
		m_Camera.AspectRatio = (float)width / (float)height;

		m_Camera.Projection = glm::ortho(-m_Camera.AspectRatio * m_Camera.Size,
										  m_Camera.AspectRatio * m_Camera.Size,
										 -m_Camera.Size,
										  m_Camera.Size);
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<MouseWheelScrolledEvent>(VN_BIND_EVENT_FN(OrthographicCameraController::OnMouseWheelScrolled));
	}

	void OrthographicCameraController::OnUpdate(float dt)
	{
		m_CameraMovementSpeed = m_Camera.Size;

		if (Input::IsKeyDown(VN_KEY_W))
		{
			m_CameraPosition.y += m_CameraMovementSpeed * dt;
		}

		if (Input::IsKeyDown(VN_KEY_S))
		{
			m_CameraPosition.y -= m_CameraMovementSpeed * dt;
		}
		
		if (Input::IsKeyDown(VN_KEY_D))
		{
			m_CameraPosition.x += m_CameraMovementSpeed * dt;
		}

		if (Input::IsKeyDown(VN_KEY_A))
		{
			m_CameraPosition.x -= m_CameraMovementSpeed * dt;
		}
	}

	bool OrthographicCameraController::OnMouseWheelScrolled(MouseWheelScrolledEvent& e)
	{
		m_Camera.Size -= e.GetYOffset() * 0.25f;
		m_Camera.Size = std::max(m_Camera.Size, 0.25f);
		m_Camera.Projection = glm::ortho(-m_Camera.AspectRatio * m_Camera.Size, 
										  m_Camera.AspectRatio * m_Camera.Size,
										 -m_Camera.Size, 
										  m_Camera.Size);
		return false;
	}
}