#pragma once

#include <rapidjson/document.h>
#include <rapidjson/prettywriter.h>
#include <glm/glm.hpp>

#include "Vision/Core/Base.h"

namespace Vision
{
    class Scene;

    class SceneSerializer
    {
    public:
        static bool Serialize(const std::string& filePath, Scene& scene);
        static bool Deserialize(const std::string& filePath, Scene& scene);

        static void SerializeEntity(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w, Scene& scene, uint32 entity);

        static void SerializeString(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            const std::string& value);

        static void SerializeBool(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            bool value);

        static void SerializeFloat(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            float value);

        static void SerializeUint32(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            uint32 value);

        static void SerializeUint64(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            uint64 value);

        static void SerializeVector2(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            const glm::vec2& values);

        static void SerializeVector3(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            const glm::vec3& values);

        static void SerializeVector4(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
            const std::string& name,
            const glm::vec4& values);

        static std::string DeserializeString(const rapidjson::Value& v,
            const std::string& name);

        static bool DeserializeBool(const rapidjson::Value& v,
            const std::string& name);

        static float DeserializeFloat(const rapidjson::Value& v,
            const std::string& name);

        static uint32 DeserializeUint32(const rapidjson::Value& v,
            const std::string& name);

        static uint64 DeserializeUint64(const rapidjson::Value& v,
            const std::string& name);

        static glm::vec2 DeserializeVector2(const rapidjson::Value& v,
            const std::string& name);

        static glm::vec3 DeserializeVector3(const rapidjson::Value& v,
            const std::string& name);

        static glm::vec4 DeserializeVector4(const rapidjson::Value& v,
            const std::string& name);

    };
}