#pragma once

#include <glm/glm.hpp>
#include <rapidjson/document.h>

#include "Vision/Core/Base.h"

namespace Vision
{
    class Scene;

    using Reader = rapidjson::Value;

    class TextDeserializer
    {
    public:
        static bool DeserializeScene(const std::string& filepath, Scene* scene);
        static void DeserializeEntity(const Reader& reader, Scene* scene, uint32 entityIndex);

        static bool DeserializeBool(const Reader& reader, const std::string& name);
        static int32 DeserializeInt32(const Reader& reader, const std::string& name);
        static uint32 DeserializeUint32(const Reader& reader, const std::string& name);
        static uint64 DeserializeInt64(const Reader& reader, const std::string& name);
        static uint64 DeserializeUint64(const Reader& reader, const std::string& name);
        static float DeserializeFloat(const Reader& reader, const std::string& name);
        static glm::vec2 DeserializeVector2(const Reader& reader, const std::string& name);
        static glm::vec3 DeserializeVector3(const Reader& reader, const std::string& name);
        static glm::vec4 DeserializeVector4(const Reader& reader, const std::string& name);
        static std::string DeserializeString(const Reader& reader, const std::string& name);
    };
}