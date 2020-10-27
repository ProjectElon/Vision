#include "pch.h"

#include "Vision/IO/TextDeserializer.h"
#include "Vision/Entity/Scene.h"

#include <fstream>

namespace Vision
{
    void TextDeserializer::DeserializeEntity(const Reader& reader, Scene* scene, uint32 entityIndex)
    {
        EditorState& editorState = Scene::EditorState;

        using namespace rapidjson;

        const Value& entity = reader[entityIndex];

        const char* tag = entity["Tag"].GetString();
        scene->CreateEntity(tag);

        const Value& components = entity["Components"];

        for (uint32 componentIndex = 0;
            componentIndex < components.Size();
            componentIndex++)
        {
            const Value& component = components[componentIndex];
            const char* name = component["Name"].GetString();
            ComponentID componentID = component["ID"].GetUint64();
            ComponentInfo& info = editorState.ComponentMeta[componentID];
            info.AddFn(entityIndex + 1);

            if (info.DeserializeFn)
            {
                const Value& data = component["Data"];
                void* componentMemory = scene->GetComponent(entityIndex + 1, componentID);
                info.DeserializeFn(data, componentMemory);
            }
        }
    }

    bool TextDeserializer::DeserializeScene(const std::string& filepath, Scene* scene)
    {
        using namespace rapidjson;

        // @Clean Up: File IO
        std::ifstream ifs(filepath);
        std::string contents;

        if (ifs.is_open())
        {
            std::string line;

            while (std::getline(ifs, line))
            {
                contents += line;
            }

            ifs.close();
        }
        else
        {
            VN_CORE_INFO("Can't Open Scene: {0}", filepath);
            return false;
        }

        EditorState& editorState = Scene::EditorState;

        Document doc;
        doc.Parse(contents.c_str(), contents.length());

        scene->MaxEntityCount = doc["MaxEntityCount"].GetUint();
        scene->PrimaryCameraTag = doc["PrimaryCamera"].GetString();
        editorState.SelectedEntityTag = doc["SelectedEntity"].GetString();

        scene->ReConstruct();

        const Value& entites = doc["Entities"];

        for (uint32 entityIndex = 0;
            entityIndex < entites.Size();
            entityIndex++)
        {
            DeserializeEntity(entites, scene, entityIndex);
        }

        VN_CORE_INFO("Deserializing: {0}", filepath);

        return true;
    }

    std::string TextDeserializer::DeserializeString(const Reader& reader,
        const std::string& name)
    {
        return reader[name.c_str()].GetString();
    }

    bool TextDeserializer::DeserializeBool(const Reader& reader, const std::string& name)
    {
        return reader[name.c_str()].GetBool();
    }

    float TextDeserializer::DeserializeFloat(const Reader& reader, const std::string& name)
    {
        return reader[name.c_str()].GetFloat();
    }

    int32 TextDeserializer::DeserializeInt32(const Reader& reader, const std::string& name)
    {
        return reader[name.c_str()].GetInt();
    }

    uint32 TextDeserializer::DeserializeUint32(const rapidjson::Value& reader, const std::string& name)
    {
        return reader[name.c_str()].GetUint();
    }

    static int64 DeserializeInt64(const Reader& reader, const std::string& name)
    {
        return reader[name.c_str()].GetInt64();
    }

    uint64 TextDeserializer::DeserializeUint64(const rapidjson::Value& reader, const std::string& name)
    {
        return reader[name.c_str()].GetUint64();
    }

    glm::vec2 TextDeserializer::DeserializeVector2(const rapidjson::Value& reader, const std::string& name)
    {
        const auto& array = reader[name.c_str()];

        glm::vec3 result;
        result.x = array[0].GetFloat();
        result.y = array[1].GetFloat();

        return result;
    }

    glm::vec3 TextDeserializer::DeserializeVector3(const rapidjson::Value& reader,
        const std::string& name)
    {
        const auto& array = reader[name.c_str()];

        glm::vec3 result;
        result.x = array[0].GetFloat();
        result.y = array[1].GetFloat();
        result.z = array[2].GetFloat();

        return result;
    }

    glm::vec4 TextDeserializer::DeserializeVector4(const rapidjson::Value& reader, const std::string& name)
    {
        const auto& array = reader[name.c_str()];

        glm::vec4 result;

        result.x = array[0].GetFloat();
        result.y = array[1].GetFloat();
        result.z = array[2].GetFloat();
        result.w = array[3].GetFloat();

        return result;
    }
}