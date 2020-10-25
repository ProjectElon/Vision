#include "pch.h"

#include "Vision/Entity/SceneSerializer.h"
#include "Vision/Entity/Scene.h"

#include <fstream>

namespace Vision
{
    bool SceneSerializer::Serialize(const std::string& filePath, Scene& scene)
    {
        EditorState& editorState = Scene::EditorState;

        using namespace rapidjson;

        StringBuffer s;
        PrettyWriter<StringBuffer> w(s);

        w.StartObject();

        SerializeString(w, "Scene", scene.Name);
        SerializeUint32(w, "MaxEntityCount", scene.MaxEntityCount);
        SerializeString(w, "PrimaryCamera", scene.PrimaryCameraTag);
        SerializeString(w, "SelectedEntity", editorState.SelectedEntityTag);

        w.Key("Entities");
        w.StartArray();

        for (uint32 entity = 1;
            entity <= scene.EntityCount;
            entity++)
        {
            SerializeEntity(w, scene, entity);
        }

        w.EndArray();

        w.EndObject();

        std::ofstream ofs(filePath);
        ofs << s.GetString();
        ofs.close();

        return true;
    }

    bool SceneSerializer::Deserialize(const std::string& filePath, Scene& scene)
    {
        using namespace rapidjson;

        EditorState& editorState = Scene::EditorState;

        std::ifstream ifs(filePath);

        std::string line;
        std::string contents;

        while (std::getline(ifs, line))
        {
            contents += line;
        }

        Document sd;
        sd.Parse(contents.c_str(), contents.length());

        scene.Name = sd["Scene"].GetString();
        scene.MaxEntityCount = sd["MaxEntityCount"].GetUint();
        scene.PrimaryCameraTag = sd["PrimaryCamera"].GetString();
        editorState.SelectedEntityTag = sd["SelectedEntity"].GetString();

        scene.ReConstruct();

        const Value& entites = sd["Entities"];

        for (uint32 entityIndex = 0;
            entityIndex < entites.Size();
            entityIndex++)
        {
            const Value& entity = entites[entityIndex];

            const char* tag = entity["Tag"].GetString();
            scene.CreateEntity(tag);

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
                    void* componentMemory = scene.GetComponent(entityIndex + 1, componentID);
                    info.DeserializeFn(data, componentMemory);
                }
            }
        }

        return true;
    }

    void SceneSerializer::SerializeEntity(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
        Scene& scene,
        Entity entity)
    {
        using namespace rapidjson;

        EditorState& editorState = Scene::EditorState;

        w.StartObject();

        auto& tagComponent = scene.GetComponent<TagComponent>(entity);
        const std::string& name = tagComponent.Tag;

        SerializeString(w, "Tag", name);

        const EntityStorage& storage = scene.m_Entities[entity];

        w.Key("Components");
        w.StartArray();

        for (const auto& [componentID, componentIndex] : storage)
        {
            // ignore tag component
            if (componentID == typeid(TagComponent).hash_code())
            {
                continue;
            }

            const ComponentInfo& info = editorState.ComponentMeta[componentID];
            w.StartObject();

            SerializeString(w, "Name", info.Name);
            SerializeUint64(w, "ID", componentID);

            if (info.SerializeFn)
            {
                w.Key("Data");
                w.StartObject();
                void* componentMemory = scene.GetComponent(entity, componentID);
                info.SerializeFn(w, componentMemory);
                w.EndObject();
            }

            w.EndObject();
        }

        w.EndArray();

        w.EndObject();
    }

    void SceneSerializer::SerializeString(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                                const std::string& name,
                                const std::string& value)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.String(value.c_str(), (rapidjson::SizeType)value.length());
    }

    void SceneSerializer::SerializeBool(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                                const std::string& name,
                                bool value)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.Bool(value);
    }

    void SceneSerializer::SerializeFloat(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                                const std::string& name,
                                float value)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.Double(value);
    }

    void SceneSerializer::SerializeUint32(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                              const std::string& name,
                              uint32 value)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.Uint(value);
    }

    void SceneSerializer::SerializeUint64(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                                const std::string& name,
                                uint64 value)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.Uint64(value);
    }

    void SceneSerializer::SerializeVector2(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                                 const std::string& name,
                                 const glm::vec2& values)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.StartArray();
        w.Double(values.x);
        w.Double(values.y);
        w.EndArray();
    }

    void SceneSerializer::SerializeVector3(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                                 const std::string& name,
                                 const glm::vec3& values)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.StartArray();
        w.Double(values.x);
        w.Double(values.y);
        w.Double(values.z);
        w.EndArray();
    }

    void SceneSerializer::SerializeVector4(rapidjson::PrettyWriter<rapidjson::StringBuffer>& w,
                                 const std::string& name,
                                 const glm::vec4& values)
    {
        w.Key(name.c_str(), (rapidjson::SizeType)name.length());
        w.StartArray();
        w.Double(values.x);
        w.Double(values.y);
        w.Double(values.z);
        w.Double(values.w);
        w.EndArray();
    }

    std::string SceneSerializer::DeserializeString(const rapidjson::Value& v,
                                const std::string& name)
    {
        return v[name.c_str()].GetString();
    }

    bool SceneSerializer::DeserializeBool(const rapidjson::Value& v,
                                const std::string& name)
    {
        return v[name.c_str()].GetBool();
    }

    float SceneSerializer::DeserializeFloat(const rapidjson::Value& v,
                                  const std::string& name)
    {
        return v[name.c_str()].GetFloat();
    }

    uint32 SceneSerializer::DeserializeUint32(const rapidjson::Value& v,
                                    const std::string& name)
    {
        return v[name.c_str()].GetUint();
    }

    uint64 SceneSerializer::DeserializeUint64(const rapidjson::Value& v,
                                    const std::string& name)
    {
        return v[name.c_str()].GetUint64();
    }

    glm::vec2 SceneSerializer::DeserializeVector2(const rapidjson::Value& v,
                                   const std::string& name)
    {
        const auto& array = v[name.c_str()];

        glm::vec3 result;
        result.x = array[0].GetFloat();
        result.y = array[1].GetFloat();

        return result;
    }

    glm::vec3 SceneSerializer::DeserializeVector3(const rapidjson::Value& v,
                                   const std::string& name)
    {
        const auto& array = v[name.c_str()];

        glm::vec3 result;
        result.x = array[0].GetFloat();
        result.y = array[1].GetFloat();
        result.z = array[2].GetFloat();

        return result;
    }

    glm::vec4 SceneSerializer::DeserializeVector4(const rapidjson::Value& v,
                                   const std::string& name)
    {
        const auto& array = v[name.c_str()];

        glm::vec4 result;

        result.x = array[0].GetFloat();
        result.y = array[1].GetFloat();
        result.z = array[2].GetFloat();
        result.w = array[3].GetFloat();

        return result;
    }

}