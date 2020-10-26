#include "Menubar.h"
#include <imgui.h>

#include "Vision/Entity/Scene.h"
#include "Vision/Entity/SceneSerializer.h"

namespace Vision
{
	void Menubar::OnImGuiRender()
	{
		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File", true))
		{
			if (ImGui::MenuItem("New Scene...", "Ctrl+Shift+N"))
			{
				m_Action = "New Scene";
			}

			if (ImGui::MenuItem("Open Scene...", "Ctrl+Shift+O"))
			{
				OpenScene();
			}

			Scene* scene = Scene::GetActiveScene();
			bool enabled = scene != nullptr;

			if (ImGui::MenuItem("Save Scene", "Ctrl+S", false, enabled))
			{
				SaveScene(*scene);
			}

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S", false, enabled))
			{
				SaveSceneAs(*scene);
			}

			if (ImGui::MenuItem("Close Scene", "Ctrl+E", false, enabled))
			{
				CloseScene(*scene);
			}

			ImGui::EndMenu();
		}

		if (m_Action == "New Scene")
		{
			ImGui::OpenPopup("Create Scene");
		}

		OpenCreateSceneDialog();

		ImGui::EndMainMenuBar();
	}

	void Menubar::OpenCreateSceneDialog()
	{
		static std::string filepath = "";
		static int32 maxEntityCount = 1;

		ImGuiWindowFlags flags = ImGuiWindowFlags_NoMove | ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking;

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
					NewScene(filepath, maxEntityCount);
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
			m_Action = "";
			filepath = "";
			maxEntityCount = 1;
		}
	}

	void Menubar::NewScene(const std::string& filepath, uint32 maxEntityCount)
	{
		Scene* scene = Scene::GetActiveScene();

		if (scene)
		{
			SceneSerializer::Serialize(scene->Path, *scene);
			delete scene;
		}

		// @Note: Clean Up File System
		size_t lastSlash = filepath.find_last_of("/\\");
		size_t lastDot = filepath.rfind('.');

		size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
		size_t count = (lastDot == std::string::npos) ? filepath.length() - start : lastDot - start;

		Scene* newScene = new Scene();
		newScene->Path = filepath;
		newScene->Name = filepath.substr(start, count);
		newScene->MaxEntityCount = maxEntityCount;

		Scene::SetActiveScene(newScene);
		SaveScene(*newScene);
	}

	void Menubar::OpenScene()
	{
		std::string filepath = FileDialog::OpenFile("Vision Scene (*.vscene)", { ".vscene" });

		if (!filepath.empty())
		{
			Scene* scene = Scene::GetActiveScene();

			if (scene)
			{
				SaveScene(*scene);
				delete scene;
			}

			// @Note: Clean Up File System
			size_t lastSlash = filepath.find_last_of("/\\");
			size_t lastDot = filepath.rfind('.');

			size_t start = (lastSlash == std::string::npos) ? 0 : lastSlash + 1;
			size_t count = (lastDot == std::string::npos) ? filepath.length() - start : lastDot - start;

			Scene* newScene = new Scene();
			newScene->Path = filepath;
			newScene->Name = filepath.substr(start, count);

			Scene::SetActiveScene(newScene);
			SceneSerializer::Deserialize(filepath, *newScene);
		}
	}

	void Menubar::SaveSceneAs(Scene& scene)
	{
		std::string filepath = FileDialog::SaveFile("Vision Scene (*.vscene)", ".vscene");

		if (!filepath.empty())
		{
			SaveScene(scene);
		}
	}

	void Menubar::SaveScene(Scene& scene)
	{
		SceneSerializer::Serialize(scene.Path, scene);
	}

	void Menubar::CloseScene(Scene& scene)
	{
		SaveScene(scene);
		delete &scene;
		Scene::SetActiveScene(nullptr);
	}

	bool Menubar::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
		{
			return false;
		}

		bool control = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);
		bool shift = Input::IsKeyDown(Key::LeftShift) || Input::IsKeyDown(Key::RightShift);

		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (control && shift)
				{
					if (m_Action.empty()) m_Action = "New Scene";
				}
			}
			break;

			case Key::S:
			{
				if (control && shift)
				{
					Scene* scene = Scene::GetActiveScene();
					if (scene) SaveSceneAs(*scene);
				}
				else if (control)
				{
					Scene* scene = Scene::GetActiveScene();
					if (scene) SaveScene(*scene);
				}
			}
			break;

			case Key::O:
			{
				if (control && shift)
				{
					OpenScene();
				}
			}
			break;

			case Key::E:
			{
				if (control)
				{
					Scene* scene = Scene::GetActiveScene();
					if (scene) CloseScene(*scene);
				}
			}
			break;
		}

		return false;
	}
}