#include "pch.h"

#include "Vision/Renderer/PerspectiveCamera.h"
#include "Vision/Platform/Input.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

namespace Vision
{
    PerspectiveCamera::PerspectiveCamera(float32 aspectRatio, float32 fieldOfView, float32 nearClip, float32 farClip)
    {
        Init(aspectRatio, fieldOfView, nearClip, farClip);
    }

    void PerspectiveCamera::Init(float32 aspectRatio, float32 fieldOfView, float32 nearClip, float32 farClip)
    {
        AspectRatio = aspectRatio;
        FieldOfView = fieldOfView;
        NearClip = nearClip;
        FarClip = farClip;

        UpdateView();
        UpdateProjection();
    }

    void PerspectiveCamera::SetViewportSize(float32 width, float32 height)
    {
        ViewportWidth  = width;
        ViewportHeight = height;
        AspectRatio    = width / height;

        UpdateProjection();
    }

    void PerspectiveCamera::UpdateView()
    {
        glm::quat orientation = glm::quat(glm::vec3(-Pitch, -Yaw, 0.0f));

        Up      = glm::rotate(orientation, glm::vec3(0.0f, 1.0f, 0.0f));
        Right   = glm::rotate(orientation, glm::vec3(1.0f, 0.0f, 0.0f));
        Forward = glm::rotate(orientation, glm::vec3(0.0f, 0.0f, -1.0f));

        Position = FocalPoint - Forward * Distance;

        glm::mat4 view = glm::translate(glm::mat4(1.0f), Position) *
                         glm::toMat4(orientation);

        View = glm::inverse(view);
    }

    void PerspectiveCamera::UpdateProjection()
    {
        Projection = glm::perspective(glm::radians(FieldOfView),
                                      AspectRatio,
                                      NearClip,
                                      FarClip);
    }

    void PerspectiveCamera::OnUpdate(float32 deltaTime)
    {
        glm::vec2 mousePosition = Input::GetMousePosition();
        glm::vec2 mouseDelta = (mousePosition - PreviousMousePosition) * 0.003f;
        PreviousMousePosition = mousePosition;

        if (Input::IsKeyDown(Key::LeftAlt))
        {
            if (Input::IsMouseButtonDown(Mouse::ButtonLeft))
            {
                Pan(mouseDelta);
            }

            if (Input::IsMouseButtonDown(Mouse::ButtonRight))
            {
                Rotate(mouseDelta);
            }
        }

        float movementSpeed = 10.0f;

        if (Input::IsKeyDown(Key::W))
        {
            FocalPoint += Forward * movementSpeed * deltaTime;
        }

        if (Input::IsKeyDown(Key::S))
        {
            FocalPoint += -Forward * movementSpeed * deltaTime;
        }

        if (Input::IsKeyDown(Key::D))
        {
            FocalPoint += Right * movementSpeed * deltaTime;
        }

        if (Input::IsKeyDown(Key::A))
        {
            FocalPoint += -Right * movementSpeed * deltaTime;
        }

        if (Input::IsKeyDown(Key::Space))
        {
            FocalPoint += Up * movementSpeed * deltaTime;
        }

        if (Input::IsKeyDown(Key::LeftShift))
        {
            FocalPoint += -Up * movementSpeed * deltaTime;
        }

        UpdateView();
    }

    void PerspectiveCamera::Zoom(float32 mouseWheeldelta)
    {
        float32 distance = std::max(Distance * 0.2f, 0.0f);
        float32 zoomSpeed = std::min(distance * distance, 100.0f);

        Distance -= mouseWheeldelta * zoomSpeed;

        if (Distance < 1.0f)
        {
            FocalPoint += Forward;
            Distance = 1.0f;
        }

        UpdateView();
    }

    void PerspectiveCamera::Rotate(const glm::vec2& mouseDelta)
    {
        float32 rotationSpeed = 0.8f;

        float yawSign = Up.y < 0 ? -1.0f : 1.0f;
        Yaw += yawSign * mouseDelta.x * rotationSpeed;
        Pitch += mouseDelta.y * rotationSpeed;
    }

    void PerspectiveCamera::Pan(const glm::vec2& mouseDelta)
    {
        float x = std::min(ViewportWidth / 1000.0f, 2.4f);
        float xSpeed = 0.0366f * (x * x) - 0.1778f * x + 0.3021f;

        float y = std::min(ViewportHeight / 1000.0f, 2.4f);
        float ySpeed = 0.0366f * (y * y) - 0.1778f * y + 0.3021f;

        FocalPoint += -Right * mouseDelta.x * xSpeed * Distance;
        FocalPoint += Up * mouseDelta.y * ySpeed * Distance;
    }

    void PerspectiveCamera::Reset()
    {
        Position = { 0.0f, 0.0f, -10.0f };
        FocalPoint = { 0.0f, 0.0f, 0.0f };

        Up = { 0.0f, 1.0f, 0.0f };
        Right = { 1.0f, 0.0f, 0.0f };
        Forward = { 0.0f, 0.0f, -1.0f };

        Distance = 10.0f;

        AspectRatio = 1.778f; // 16:9
        FieldOfView = 45.0f;

        NearClip = 0.1f;
        FarClip  = 1000.0f;

        Pitch = 0.0f;
        Yaw = 0.0f;
    }
}