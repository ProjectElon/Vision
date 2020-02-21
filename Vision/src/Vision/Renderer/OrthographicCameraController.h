#pragma once

#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/MouseEvent.h"

namespace Vision
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, float zoomLevel = 10.0f, bool rotation = false);

		void OnEvent(Event& e);
		void OnUpdate(float dt);
		void OnImGuiRender();

		inline void SetRotation(bool rotation) { m_Rotation = rotation; }

		inline const OrthographicCamera& GetCamera() const { return m_Camera; }
		
	private:
		float m_AspectRatio;
		float m_ZoomLevel;
		bool m_Rotation;

		OrthographicCamera m_Camera;

		float m_CameraMovementSpeed;
		float m_CameraRotationSpeed = 10.0f;

		glm::vec3 m_CameraPosition = glm::vec3(0.0f);
		float m_CameraRotation = 0.0f;

	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseWheelScrolled(MouseWheelScrolledEvent& e);
	};
}