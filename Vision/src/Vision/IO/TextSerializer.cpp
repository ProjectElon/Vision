#include "pch.h"

#include "Vision/IO/TextSerializer.h"
#include "Vision/Entity/Scene.h"

#include <fstream>

namespace Vision
{
    bool TextSerializer::SerializeScene(const std::string& filepath, Scene* scene)
    {
        // @Clean Up: File IO
        std::ofstream ofs(filepath);

        if (!ofs.is_open())
        {
            VN_CORE_INFO("Can't Open File: {0}", filepath);
            return false;
        }

        EditorState& editorState = Scene::EditorState;

        using namespace rapidjson;

        StringBuffer s;
        PrettyWriter<StringBuffer> writer(s);

        writer.StartObject();

        SerializeUint32(writer, "MaxEntityCount", scene->MaxEntityCount);
        SerializeString(writer, "PrimaryCamera",  scene->PrimaryCameraTag);
        SerializeString(writer, "SelectedEntity", editorState.SelectedEntityTag);

        writer.Key("Entities");
        writer.StartArray();

        for (uint32 entity = 1;
            entity <= scene->EntityCount;
            entity++)
        {
            SerializeEntity(writer, scene, entity);
        }

        writer.EndArray();

        writer.EndObject();

        ofs << s.GetString();
        ofs.close();

        VN_CORE_INFO("Serializing: {0}", filepath);

        return true;
    }

    void TextSerializer::SerializeEntity(Writer& writer,
        Scene* scene,
        Entity entity)
    {
        using namespace rapidjson;

        EditorState& editorState = Scene::EditorState;

        writer.StartObject();

        auto& tagComponent = scene->GetComponent<TagComponent>(entity);
        const std::string& name = tagComponent.Tag;

        SerializeString(writer, "Tag", name);

        const EntityStorage& storage = scene->m_Entities[entity];

        writer.Key("Components");
        writer.StartArray();

        for (const auto& [componentID, componentIndex] : storage)
        {
            // ignore tag component
            if (componentID == typeid(TagComponent).hash_code())
            {
                continue;
            }

            const ComponentInfo& info = editorState.ComponentMeta[componentID];
            writer.StartObject();

            SerializeString(writer, "Name", info.Name);
            SerializeUint64(writer, "ID", componentID);

            if (info.SerializeFn)
            {
                writer.Key("Data");
                writer.StartObject();
                void* componentMemory = scene->GetComponent(entity, componentID);
                info.SerializeFn(writer, componentMemory);
                writer.EndObject();
            }

            writer.EndObject();
        }

        writer.EndArray();

        writer.EndObject();
    }

    void TextSerializer::SerializeString(Writer& writer, const std::string& name, const std::string& value)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.String(value.c_str(), (rapidjson::SizeType)value.length());
    }

    void TextSerializer::SerializeBool(Writer& writer, const std::string& name, bool value)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.Bool(value);
    }

    void TextSerializer::SerializeFloat(Writer& writer, const std::string& name, float value)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.Double(value);
    }

    void TextSerializer::SerializeInt32(Writer& writer, const std::string& name, int32 value)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.Int(value);
    }

    void TextSerializer::SerializeUint32(Writer& writer, const std::string& name, uint32 value)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.Uint(value);
    }

    void TextSerializer::SerializeInt64(Writer& writer, const std::string& name, uint64 value)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.Int64(value);
    }

    void TextSerializer::SerializeUint64(Writer& writer, const std::string& name, uint64 value)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.Uint64(value);
    }

    void TextSerializer::SerializeVector2(Writer& writer, const std::string& name, const glm::vec2& values)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.StartArray();
        writer.Double(values.x);
        writer.Double(values.y);
        writer.EndArray();
    }

    void TextSerializer::SerializeVector3(Writer& writer, const std::string& name, const glm::vec3& values)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.StartArray();
        writer.Double(values.x);
        writer.Double(values.y);
        writer.Double(values.z);
        writer.EndArray();
    }

    void TextSerializer::SerializeVector4(Writer& writer, const std::string& name, const glm::vec4& values)
    {
        writer.Key(name.c_str(), (rapidjson::SizeType)name.length());
        writer.StartArray();
        writer.Double(values.x);
        writer.Double(values.y);
        writer.Double(values.z);
        writer.Double(values.w);
        writer.EndArray();
    }
}