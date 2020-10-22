#pragma once

namespace Vision
{
    class SceneHierarchyPanel
    {
    public:
        void OnImGuiRender();

        inline bool IsInteractable() { return m_IsInteractable; }

    private:
        bool m_IsInteractable;

    private:
        void DrawEntity(Entity entity);
    };
}