#pragma once

#include "Vision/Entity/Scene.h"

namespace Vision
{
    void SerializeScene(const std::string& filepath, Scene* scene);
    std::string SerializeEntity(Scene* scene, Entity entity);
}