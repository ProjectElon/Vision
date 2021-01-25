#pragma once

namespace Vision
{
    class EditorLayer;

    enum class DialogType
    {
        None,
        CreateScene
    };

    class Dialog
    {
    public:
        void SetEditor(EditorLayer* editor);
        void OnImGuiRender();

        static void Open(DialogType type);

    private:
        void CreateSceneDialog();

        static DialogType s_Type;
        EditorLayer* m_Editor;
    };
}