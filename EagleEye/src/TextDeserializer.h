#pragma once

#include "Vision/Entity/Scene.h"

namespace Vision
{
    void DeserializeScene(const std::string& filepath, Scene* scene);
    void DeserializeComponent(ComponentID componentID,
                              const std::string& contents,
                              Scene* scene,
                              Entity entity);
}
