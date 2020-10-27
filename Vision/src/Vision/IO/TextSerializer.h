#pragma once

#include <glm/glm.hpp>
#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>

#include "Vision/Core/Base.h"

namespace Vision
{
    class Scene;

    using Writer = rapidjson::PrettyWriter<rapidjson::StringBuffer>;
    
    class TextSerializer
    {
    public:
        static void SerializeEntity(Writer& writer, Scene* scene, uint32 entity);
        static bool SerializeScene(const std::string& filepath, Scene* scene);
        
        static void SerializeBool(Writer& writer, const std::string& name, bool value);
        static void SerializeInt32(Writer& writer, const std::string& name, int32 value);
        static void SerializeUint32(Writer& writer, const std::string& name, uint32 value);
        static void SerializeInt64(Writer& writer, const std::string& name, uint64 value);
        static void SerializeUint64(Writer& writer, const std::string& name, uint64 value);
        static void SerializeFloat(Writer& writer, const std::string& name, float value);
        static void SerializeVector2(Writer& writer, const std::string& name, const glm::vec2& values);
        static void SerializeVector3(Writer& writer, const std::string& name, const glm::vec3& values);
        static void SerializeVector4(Writer& writer, const std::string& name, const glm::vec4& values);
        static void SerializeString(Writer& writer, const std::string& name, const std::string& value);
    };
}