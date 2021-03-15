#include "Dialog.h"

#include <imgui.h>

#include "EditorLayer.h"
#include "Vision/Entity/Scene.h"

namespace Vision
{
	Dialog::Dialog(EditorLayer* editor)
        : Editor(editor)
    {
    }

    void Dialog::Open(Vision::DialogType dialogType)
    {
		DialogType = dialogType;
    }

    void Dialog::OnImGuiRender()
    {
        switch (DialogType)
        {
            case DialogType::CreateScene:
            {
                ImGui::OpenPopup("Create Scene");
                CreateSceneDialog();
                break;
            }
        }
    }

    void Dialog::CreateSceneDialog()
    {
		static std::string filepath = "";
		static int32 maxEntityCount = 1;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove |
                                 ImGuiWindowFlags_AlwaysAutoResize |
                                 ImGuiWindowFlags_NoDocking;

		bool open = true;

		if (ImGui::BeginPopupModal("Create Scene", &open, flags))
		{
			ImGui::Text("Max Entity Count");
			ImGui::SameLine();

			ImGui::DragInt("##Max Entity Count Drag", &maxEntityCount, 1.0f, 1, MAXINT);

			ImGui::Text("Location");
			ImGui::InputText("##Location", filepath.data(), filepath.length() + 1);
			ImGui::SameLine();

			if (ImGui::Button("..."))
			{
				filepath = FileDialog::SaveFile("Vision Scene (*.vscene)", ".vscene");
			}

			if (!filepath.empty())
			{
				if (ImGui::Button("Create"))
				{
					Editor->NewScene(filepath, maxEntityCount);
					ImGui::CloseCurrentPopup();
					open = false;
				}

				ImGui::SameLine();
			}

			if (ImGui::Button("Cancel"))
			{
				ImGui::CloseCurrentPopup();
				open = false;
			}

			ImGui::EndPopup();
		}

		if (!open)
		{
			DialogType = DialogType::None;
			filepath = "";
			maxEntityCount = 1;
		}
    }

	DialogType Dialog::DialogType = DialogType::None;
}