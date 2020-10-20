#pragma once

namespace Vision
{
    class SceneHierarchyPanel
    {
    public:
        void OnImGuiRender();
        bool IsInteractable() { return m_IsInteractable; }

    private:
        bool m_IsInteractable;
    };
}