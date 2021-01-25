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
        stringStream << "PrimaryCamera "  << scene->PrimaryCameraTag << "\n";
        stringStream << "SelectedEntity " << editorState.SelectedEntityTag << "\n";

        const OrthographicCamera& sceneCamera = editorState.SceneCamera;
        stringStream << "CameraPosition "
                     << sceneCamera.Position.x << " " << sceneCamera.Position.y << " " << sceneCamera.Position.z << "\n";
        stringStream << "CameraMovementSpeed " << sceneCamera.MovementSpeed << "\n";
        stringStream << "CameraOrthographicSize " << sceneCamera.OrthographicSize << "\n";
        stringStream << "CameraNear " << sceneCamera.Near << "\n";
        stringStream << "CameraFar " << sceneCamera.Far << "\n";

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