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
			if (ImGui::MenuItem("New Scene", "Ctrl+Shift+N"))
			{
				NewScene();
			}

			if (ImGui::MenuItem("Open Scene...", "Ctrl+Shift+O"))
			{
				OpenScene();
			}

			
			Scene* scene = Scene::GetActiveScene();
			bool enabled = scene != nullptr;

			if (ImGui::MenuItem("Save Scene As...", "Ctrl+Shift+S", false, enabled))
			{
				SaveSceneAs(*scene);
			}

			if (ImGui::MenuItem("Exit"))
			{
				Application& app = Application::Get();
				app.Close();
			}

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();
	}

	void Menubar::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VN_BIND_EVENT_FN(Menubar::OnKeyPressed));
	}

	void Menubar::NewScene()
	{
		Scene* scene = Scene::GetActiveScene();

		if (scene)
		{
			SceneSerializer::Serialize(scene->Name, *scene);
			delete scene;
		}

		Scene* newScene = new Scene();
		newScene->ReConstruct();

		Scene::SetActiveScene(newScene);
	}

	void Menubar::OpenScene()
	{
		std::string filepath = FileDialog::OpenFile("Vision Scene (*.vscene)\0*.vscene\0");

		if (!filepath.empty())
		{
			Scene* scene = Scene::GetActiveScene();

			if (scene)
			{
				SceneSerializer::Serialize(scene->Name, *scene);
				delete scene;
			}

			Scene* newScene = new Scene();
			Scene::SetActiveScene(newScene);

			SceneSerializer::Deserialize(filepath, *newScene);
		}
	}

	void Menubar::SaveSceneAs(Scene& scene)
	{
		std::string filepath = FileDialog::SaveFile("Vision Scene (*.vscene)\0*.vscene\0");

		if (!filepath.empty())
		{
			uint32 lastDot = filepath.find_last_of('.');
			
			if (lastDot == -1)
			{
				filepath += ".vscene";
			}

			SceneSerializer::Serialize(filepath, scene);	
		}
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
					NewScene();
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
		}

		return false;
	}
}