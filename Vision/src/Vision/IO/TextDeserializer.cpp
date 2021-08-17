#include "pch.hpp"

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

        uint32 headerLineCount = 7;

        for (uint32 lineIndex = 0; lineIndex < headerLineCount; lineIndex++)
        {
            File::ReadLine(handle, line);
            contents += line + " ";
        }

        EditorState& editorState = Scene::EditorState;

        std::stringstream stringStream(contents);
        stringStream >> prop >> scene->MaxEntityCount;
        stringStream >> prop >> scene->PrimaryCameraTag;
        stringStream >> prop >> editorState.SelectedEntityTag;

        if (scene->PrimaryCameraTag == "none")
        {
            scene->PrimaryCameraTag = "";
        }

        if (editorState.SelectedEntityTag == "none")
        {
            editorState.SelectedEntityTag = "";
        }

        CreateScene(scene, scene->MaxEntityCount);

        PerspectiveCamera& camera = editorState.SceneCamera;
        glm::vec3& p = camera.FocalPoint;
        stringStream >> prop >> p.x >> p.y >> p.z;

        float32& pitch = camera.Pitch;
        stringStream >> prop >> pitch;

        float32& yaw = camera.Yaw;
        stringStream >> prop >> yaw;

        float32& zoomLevel = camera.Distance;
        stringStream >> prop >> zoomLevel;

        camera.UpdateView();
        camera.UpdateProjection();

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