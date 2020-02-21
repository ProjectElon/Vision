#include "pch.h"
#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Platform/Input.h"

namespace Vision
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_Position(glm::vec3(0.0f, 0.0f, 0.0f))
		, m_Rotation(0.0f)
		, m_Projection(glm::ortho(left, right, bottom, top, -1.0f, 1.0f))
		, m_View(glm::mat4(1.0f))
	{
		m_ViewProjection = m_Projection * m_View;
	}
	
	OrthographicCamera::~OrthographicCamera()
	{
	}

	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_Projection = glm::ortho(left, right, bottom, top, -1.0f, 1.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::ReCalculateView() 
	{
		glm::mat4 translation = glm::translate(glm::mat4(1.0f), -m_Position);
		glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(-m_Rotation), glm::vec3(0.0f, 0.0f, 1.0f));

		m_View = translation * rotation;
		m_ViewProjection = m_Projection * m_View;
	}
}