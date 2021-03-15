#include "pch.h"
#include "Vision/IO/TextSerializer.h"
#include "Vision/IO/File.h"
#include <sstream>

namespace Vision
{
    void SerializeScene(const std::string& filepath, Scene* scene)
    {
        EditorState& editorState = Scene::EditorState;

        FileStream handle = File::Open(filepath, FileMode::Write);
        std::stringstream stringStream;
        stringStream << "MaxEntityCount " << scene->MaxEntityCount << "\n";

        if (scene->PrimaryCameraTag.empty())
        {
            stringStream << "PrimaryCamera "  << "none" << "\n";
        }
        else
        {
            stringStream << "PrimaryCamera "  << scene->PrimaryCameraTag << "\n";
        }

        if (editorState.SelectedEntityTag.empty())
        {
            stringStream << "SelectedEntity " << "none" << "\n";
        }
        else
        {
            stringStream << "SelectedEntity " << editorState.SelectedEntityTag << "\n";
        }

        const PerspectiveCamera& sceneCamera = editorState.SceneCamera;
        stringStream << "CameraPosition "
                     << sceneCamera.FocalPoint.x << " " << sceneCamera.FocalPoint.y << " " << sceneCamera.FocalPoint.z << "\n";

        for (uint32 entity = 1; entity <= scene->EntityCount; entity++)
        {
            const auto& tagComponent = scene->GetComponent<TagComponent>(entity);
            stringStream << "Entity " << tagComponent.Tag << "\n";
            std::string entityContents = SerializeEntity(scene, entity);
            stringStream << entityContents;
        }

        File::WriteContents(handle, stringStream.str());
        File::Close(handle);
    }

    std::string SerializeEntity(Scene* scene, Entity entity)
    {
        EditorState& editorState = Scene::EditorState;

        const EntityStorage& storage = scene->Entities[entity];

        std::stringstream stringStream;

        for (const auto& [componentID, componentIndex] : storage)
        {
            if (componentID == typeid(TagComponent).hash_code())
            {
                continue;
            }

            const ComponentInfo& info = editorState.ComponentMeta[componentID];

            if (info.SerializeFn)
            {
                stringStream << "Component " << componentID << " " << info.Name << "\n";
                void* componentMemory = scene->GetComponent(entity, componentID);
                stringStream << info.SerializeFn(componentMemory);
            }
        }

        return stringStream.str();
    }
}