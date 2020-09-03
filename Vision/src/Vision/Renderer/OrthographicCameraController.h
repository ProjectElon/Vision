#pragma once

#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/MouseEvent.h"

namespace Vision
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraController(float aspectRatio, float zoomLevel = 1.0f);

		void Resize(uint32 width, uint32 height);	
		void OnEvent(Event& e);
		void OnUpdate(float dt);
		
		inline const OrthographicCamera& GetCamera() const { return m_Camera; }
		inline const float GetZoomLevel() const { return m_ZoomLevel; }

	private:
		float m_AspectRatio;
		float m_ZoomLevel;
		
		float m_CameraMovementSpeed = 1.0f;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		
		OrthographicCamera m_Camera;
		
	private:
		bool OnWindowResize(WindowResizeEvent& e);
		bool OnMouseWheelScrolled(MouseWheelScrolledEvent& e);
	};
}