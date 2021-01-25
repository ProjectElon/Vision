#include "pch.h"

#include "Vision/IO/TextDeserializer.h"
#include "Vision/IO/File.h"

#include <sstream>

namespace Vision
{
    void DeserializeScene(const std::string& filepath, Scene* scene)
    {
        FileStream handle = File::Open(filepath, FileMode::Read);
        std::string contents = "";
        std::string line;

        std::string newline;
        std::string prop;
        std::string space;

        for (int i = 0; i < 8; i++)
        {
            File::ReadLine(handle, line);
            contents += line + " ";
        }

        EditorState& editorState = Scene::EditorState;

        std::stringstream stringStream(contents);
        stringStream >> prop >> scene->MaxEntityCount;
        stringStream >> prop >> scene->PrimaryCameraTag;
        stringStream >> prop >> editorState.SelectedEntityTag;

        CreateScene(scene, scene->MaxEntityCount);

        OrthographicCamera& camera = editorState.SceneCamera;
        glm::vec3& p = camera.Position;
        stringStream >> prop >> p.x >> p.y >> p.z;
        stringStream >> prop >> camera.MovementSpeed;
        stringStream >> prop >> camera.OrthographicSize;
        stringStream >> prop >> camera.Near;
        stringStream >> prop >> camera.Far;

        contents = "";
        Entity entity = 0;
        ComponentID componentID = 0;

        while (File::ReadLine(handle, line))
        {
            std::stringstream stringStream(line);
            stringStream >> prop;

            if (prop == "Entity")
            {
                if (!contents.empty())
                {
                    DeserializeComponent(componentID, contents, scene, entity);
                    contents = "";
                }

                std::string tag = line.substr(7);
                entity = scene->CreateEntity(tag);
            }
            else if (prop == "Component")
            {
                if (!contents.empty())
                {
                    DeserializeComponent(componentID, contents, scene, entity);
                    contents = "";
                }
                stringStream >> componentID;
            }
            else
            {
                contents += line + " ";
            }
        }

        if (!contents.empty())
        {
            DeserializeComponent(componentID, contents, scene, entity);
            contents = "";
        }

        File::Close(handle);
    }

    void DeserializeComponent(ComponentID componentID,
                              const std::string& contents,
                              Scene* scene,
                              Entity entity)
    {
        EditorState& editorState = Scene::EditorState;
        const ComponentInfo& info = editorState.ComponentMeta[componentID];
        info.AddFn(scene, entity);
        if (info.DeserializeFn)
        {
            void* componentMemory = scene->GetComponent(entity, componentID);
            info.DeserializeFn(componentMemory, contents);
        }
    }
}