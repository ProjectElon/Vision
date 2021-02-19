#pragma once

#include "Vision/Core/Common.h"

#include <glm/glm.hpp>

namespace Vision
{
    struct PerspectiveCamera
    {
        glm::vec3 Position = { 0.0f, 0.0f, -10.0f };
        glm::vec3 FocalPoint = { 0.0f, 0.0f, 0.0f };

        glm::vec3 Up = { 0.0f, 1.0f, 0.0f };
        glm::vec3 Right = { 1.0f, 0.0f, 0.0f };
        glm::vec3 Forward { 0.0f, 0.0f, -1.0f };

        float32 Distance = 10.0f;

        float32 AspectRatio = 1.778f; // 16:9
        float32 FieldOfView = 45.0f;

        float32 NearClip = 0.1f;
        float32 FarClip  = 1000.0f;

        float32 Pitch = 0.0f;
        float32 Yaw = 0.0f;

        glm::mat4 View = glm::mat4(1);
        glm::mat4 Projection = glm::mat4(1);

        PerspectiveCamera() = default;
        PerspectiveCamera(float32 aspectRatio, float32 fieldOfView, float32 near = 0.1f, float32 far = 1000.0f);

        void Init(float32 aspectRatio, float32 fieldOfView, float32 near = 0.1f, float32 far = 1000.0f);

        void SetViewportSize(float32 width, float32 height);

        void UpdateView();
        void UpdateProjection();

        void OnUpdate(float32 deltaTime);

        void Zoom(float32 mouseWheeldelta);
        void Rotate(const glm::vec2& mouseDelta);
        void Pan(const glm::vec2& mouseDelta);

        void Reset();

    private:
        glm::vec2 PreviousMousePosition = { 0.0f, 0.0f };

        float32 ViewportWidth = 1280;
        float32 ViewportHeight = 720;
    };
}
