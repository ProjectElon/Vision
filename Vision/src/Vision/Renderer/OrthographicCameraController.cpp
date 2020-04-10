#include "pch.h"
#include "Vision/Renderer/OrthographicCameraController.h"
#include "Vision/Core/Core.h"
#include "Vision/Platform/Input.h"

namespace Vision
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, float zoomLevel)
		: m_AspectRatio(aspectRatio)
		, m_ZoomLevel(zoomLevel)
		, m_Camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel)
	{
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(VN_BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
		dispatcher.Dispatch<MouseWheelScrolledEvent>(VN_BIND_EVENT_FN(OrthographicCameraController::OnMouseWheelScrolled));
	}

	void OrthographicCameraController::OnUpdate(float dt)
	{
		m_CameraMovementSpeed = m_ZoomLevel;

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

		m_Camera.SetPosition(m_CameraPosition);
	}

	bool OrthographicCameraController::OnWindowResize(WindowResizeEvent& e)
	{
		m_AspectRatio = (float)e.GetWidth() / (float)e.GetHeight();
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}

	bool OrthographicCameraController::OnMouseWheelScrolled(MouseWheelScrolledEvent& e)
	{
		m_ZoomLevel -= e.GetYOffset() * 0.25f;
		m_ZoomLevel = std::max(m_ZoomLevel, 0.25f);
		m_Camera.SetProjection(-m_AspectRatio * m_ZoomLevel, m_AspectRatio * m_ZoomLevel, -m_ZoomLevel, m_ZoomLevel);
		return false;
	}
}