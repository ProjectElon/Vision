#pragma once

namespace Vision
{
    struct Scene;

    class SceneHierarchyPanel
    {
    public:
        void SetActiveScene(AssetID scene);

        void OnImGuiRender();

        inline bool IsInteractable() { return m_IsInteractable; }

    private:
        AssetID m_ActiveScene = 0;
        bool m_IsInteractable = false;

    private:
        void DrawEntity(Scene* scene, Entity entity);
    };
}