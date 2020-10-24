#pragma once

#include <rapidjson/document.h>
#include <rapidjson/writer.h>
#include <rapidjson/prettywriter.h>
#include "Vision/Core/Base.h"

namespace Vision
{
    class Scene;

    class SceneSerializer
    {
    public:
        static bool Serialize(const std::string& filePath, Scene& scene);
        static bool Deserialize(const std::string& filePath, Scene& scene);

    private:
        static void SerializeEntity(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w, Scene& scene, uint32 entity);
    };
}