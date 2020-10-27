#pragma once

namespace Vision
{
    class EditorLayer;
    class Scene;

    enum class DialogType
    {
        None,
        CreateScene
    };

    class Dialog
    {
    public:
        void SetEditorLayer(EditorLayer* editorLayer);
        void OnImGuiRender();

        static void Open(DialogType type);
    
    private:
        void CreateSceneDialog();

        static DialogType s_Type;
        EditorLayer* m_EditorLayer;
    };
}