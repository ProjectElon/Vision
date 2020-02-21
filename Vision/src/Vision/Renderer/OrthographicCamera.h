#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	class OrthographicCamera
	{
	public:
		OrthographicCamera(float left, float right, float bottom, float top);
		~OrthographicCamera();

		void SetProjection(float left, float right, float bottom, float top);

		inline void SetPosition(const glm::vec3& position) { m_Position = position; ReCalculateView(); }
		inline const glm::vec3& GetPosition() const { return m_Position; }

		inline void SetRotation(float rotation) { m_Rotation = rotation; ReCalculateView(); }
		inline float GetRotation() const { return m_Rotation; }

		inline const glm::mat4& GetView() const { return m_View; }
		inline const glm::mat4& GetProjection() const { return m_Projection; }
		inline const glm::mat4& GetViewProjection() const { return m_ViewProjection; }

	private:
		glm::vec3 m_Position;
		float m_Rotation;

		glm::mat4 m_Projection;
		glm::mat4 m_View;
		glm::mat4 m_ViewProjection;

	private:
		void ReCalculateView();
	};
}