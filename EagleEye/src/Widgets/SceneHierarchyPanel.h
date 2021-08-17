#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Entity/Scene.h"
#include "Vision/IO/Assets.h"

namespace Vision
{
    class SceneHierarchyPanel
    {
    public:
        AssetID ActiveSceneID = 0;
        bool IsInteractable = false;

        void OnImGuiRender();

    private:
        void DrawEntity(Scene* scene, Entity entity);
    };
}