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
        EditorLayer* Editor;

        Dialog() = default;
        Dialog(EditorLayer* editor);

        void OnImGuiRender();

        static void Open(DialogType type);

    private:
        static DialogType DialogType;

        void CreateSceneDialog();
    };
}