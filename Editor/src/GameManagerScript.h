#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Script.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
    class GameManagerScript : public Script 
    {
    public:
        float Speed = 10.0f;

        void OnCreate();
        void OnDestroy();
        void OnUpdate(float deltaTime);

    private:
        glm::vec4* m_Position;
    };
}