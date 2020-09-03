#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
    class Camera
    {
    public:
        Camera();

        void SetTransform(const glm::mat4& transform);

        void SetProjection(float left, float right, float top, float bottom, float near, float far);
        void SetProjection(float fieldOfViewAngle, float aspectRatio, float near, float far);

        const glm::mat4& GetView() const
        {
            return m_View;
        }

        const glm::mat4& GetProjection() const
        {
            return m_Projection;
        }

        const glm::vec3& GetPosition() const
        {
            return m_View[3];
        }

    private:
        glm::mat4 m_View;
        glm::mat4 m_Projection;
    };
}