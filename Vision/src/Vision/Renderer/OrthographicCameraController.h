#pragma once

#include "Vision/Events/ApplicationEvent.h"
#include "Vision/Events/MouseEvent.h"
#include "Vision/Entity/Components.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	class OrthographicCameraController
	{
	public:
		OrthographicCameraComponent Camera;
		
		OrthographicCameraController(float aspectRatio, float Size = 1.0f, float Near = -1.0f, float Far = 1.0f);

		void Resize(uint32 width, uint32 height);	
		
		void OnEvent(Event& e);
		void OnUpdate(float dt);
		
		inline glm::mat4 GetCameraTransform() const { return glm::translate(glm::mat4(1.0f), m_CameraPosition); }
		
	private:
		float m_CameraMovementSpeed = 1.0f;
		glm::vec3 m_CameraPosition = { 0.0f, 0.0f, 0.0f };
		
	private:
		bool OnMouseWheelScrolled(MouseWheelScrolledEvent& e);
	};
}