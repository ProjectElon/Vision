#include "pch.h"
#include "Vision/Renderer/Camera.h"

namespace Vision
{
    Camera::Camera()
        : m_View(glm::mat4(1.0f))
        , m_Projection(glm::mat4(1.0f))
    {
    }

    void Camera::SetTransform(const glm::mat4& transform)
    {
        m_View = glm::inverse(transform);
    }

    void Camera::SetProjection(float left, float right, float top, float bottom, float near, float far)
    {
        m_Projection = glm::ortho(left, right, bottom, top, near, far);
    }

    void Camera::SetProjection(float fieldOfViewAngle, float aspectRatio, float near, float far)
    {
        m_Projection = glm::perspective(fieldOfViewAngle, aspectRatio, near, far);
    }
}