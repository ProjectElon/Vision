#include "pch.h"
#include "Vision/Renderer/OrthographicCameraController.h"
#include "Vision/Core/Core.h"
#include "Vision/Platform/Input.h"

#include <imgui.h>

namespace Vision
{
	OrthographicCameraController::OrthographicCameraController(float aspectRatio, float zoomLevel, bool rotation)
		: m_AspectRatio(aspectRatio)
		, m_ZoomLevel(zoomLevel)
		, m_Rotation(rotation)
		, m_Camera(-aspectRatio * zoomLevel, aspectRatio * zoomLevel, -zoomLevel, zoomLevel)
	{
	}

	void OrthographicCameraController::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OrthographicCameraController::OnWindowResize));
		dispatcher.Dispatch<MouseWheelScrolledEvent>(BIND_EVENT_FN(OrthographicCameraController::OnMouseWheelScrolled));
	}

	void OrthographicCameraController::OnUpdate(float dt)
	{
		m_CameraMovementSpeed = m_ZoomLevel;

		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(m_CameraRotation), glm::vec3(0.0f, 0.0f, 1.0f));
		glm::vec3 up = rotation * glm::vec4(0.0f, 1.0f, 0.0f, 0.0f);
		glm::vec3 right = rotation * glm::vec4(1.0f, 0.0f, 0.0f, 0.0f);

		if (Input::IsKeyDown(VN_KEY_W))
		{
			m_CameraPosition += up * m_CameraMovementSpeed * dt;
		}

		if (Input::IsKeyDown(VN_KEY_S))
		{
			m_CameraPosition -= up * m_CameraMovementSpeed * dt;
		}

		if (Input::IsKeyDown(VN_KEY_A))
		{
			m_CameraPosition -= right * m_CameraMovementSpeed * dt;
		}

		if (Input::IsKeyDown(VN_KEY_D))
		{
			m_CameraPosition += right * m_CameraMovementSpeed * dt;
		}

		if (m_Rotation)
		{
			if (Input::IsKeyDown(VN_KEY_Q))
			{
				m_CameraRotation -= m_CameraRotationSpeed * dt;
			}

			if (Input::IsKeyDown(VN_KEY_E))
			{
				m_CameraRotation += m_CameraRotationSpeed * dt;
			}

			while (m_CameraRotation > 360.0f)
			{
				m_CameraRotation -= 360.0f;
			}

			while (m_CameraRotation < -360.0f)
			{
				m_CameraRotationSpeed += 360.0f;
			}

			m_Camera.SetRotation(m_CameraRotation);
		}

		m_Camera.SetPosition(m_CameraPosition);
	}

	void OrthographicCameraController::OnImGuiRender()
	{
		ImGui::Begin("Orthographic Camera Controls");
		
		ImGui::Checkbox("Rotation", &m_Rotation);
		ImGui::SliderFloat("Rotation Speed", &m_CameraRotationSpeed, 0.0f, 360.0f);

		ImGui::End();
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