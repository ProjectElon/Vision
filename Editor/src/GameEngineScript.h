#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Entity/Script.h"
#include "Vision/Entity/Components.h"

namespace Vision
{
    class GameEngineScript : public Script
    {
    public:
        std::string Message = "Unkown";

        void OnCreate();
        void OnDestroy();
        void OnUpdate(float deltaTime);
    };
}