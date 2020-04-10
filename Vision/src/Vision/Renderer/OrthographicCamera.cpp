#include "pch.h"
#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Platform/Input.h"

namespace Vision
{
	OrthographicCamera::OrthographicCamera(float left, float right, float bottom, float top)
		: m_Position(0.0f, 0.0f, 0.1f)
		, m_Projection(glm::ortho(left, right, bottom, top, 0.0f, 10000.0f))
		, m_View(glm::mat4(1.0f))
	{
		m_ViewProjection = m_Projection * m_View;
	}
	
	void OrthographicCamera::SetProjection(float left, float right, float bottom, float top)
	{
		m_Projection = glm::ortho(left, right, bottom, top, 0.0f, 10000.0f);
		m_ViewProjection = m_Projection * m_View;
	}

	void OrthographicCamera::ReCalculateView() 
	{
		m_View = glm::translate(glm::mat4(1.0f), -m_Position);
		m_ViewProjection = m_Projection * m_View;
	}
}