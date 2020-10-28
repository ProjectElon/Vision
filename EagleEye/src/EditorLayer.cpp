#include "EditorLayer.h"
#include <fstream>
#include <imgui.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	EditorLayer::EditorLayer()
		: Layer("Editor")
	{
		m_Window = &Application::Get().GetWindow();
		m_Window->SetVSync(true);
		LoadSettings();
	}

	EditorLayer::~EditorLayer()
	{
		SaveSettings();
	}

	void EditorLayer::OnAttach()
	{
		using namespace Vision;

		FrameBufferProps frameBufferProps;
		frameBufferProps.Width = 800;
		frameBufferProps.Height = 600;

		m_SceneFrameBuffer = FrameBuffer::Create(frameBufferProps);
		m_GameFrameBuffer = FrameBuffer::Create(frameBufferProps);

		m_SceneViewPanel.SetFrameBuffer(m_SceneFrameBuffer.get());
		m_GameViewPanel.SetFrameBuffer(m_GameFrameBuffer.get());

		// @Clean up: Should be storeed with the scene
		float32 aspectRatio = (float32)frameBufferProps.Width / (float32)frameBufferProps.Height;
		using TDS = TextDeserializer;
		m_CameraController = CreateScope<OrthographicCameraController>(aspectRatio, TDS::DeserializeFloat(m_Settings, "CameraZoomLevel"));

		m_SpriteShader = Shader::CreateFromFile("Assets/Shaders/Sprite.glsl");

		// Texture Properties
		TextureProps tiled;
		tiled.WrapX = WrapMode::Repeat;
		tiled.WrapY = WrapMode::Repeat;
		tiled.FilterMode = FilterMode::Point;

		m_CheckboardTexture = Texture2D::CreateFromFile("Assets/Textures/Checkerboard.png", tiled);

		std::string lastScenePath = TDS::DeserializeString(m_Settings, "Last Scene Path");

		if (!lastScenePath.empty())
		{
			Scene* scene = new Scene();
			scene->Path = lastScenePath;
			scene->Name = FileSystem::GetFileName(lastScenePath, false);

			Scene::SetActiveScene(scene);
			TDS::DeserializeScene(lastScenePath, scene);
		}

		m_Menubar.SetEditorLayer(this);
		m_Dialog.SetEditorLayer(this);

		std::vector<std::string> result = FileSystem::ScanDirectory("Assets", {}, true);
		
		for (const auto& cur : result)
		{
			VN_CORE_TRACE(cur);
		}


	}

	void EditorLayer::OnDetach()
	{
		Scene* scene = Scene::GetActiveScene();
		m_LastScenePath = "";

		if (scene)
		{
			m_LastScenePath = scene->Path;
			TextSerializer::SerializeScene(m_LastScenePath, scene);
			delete scene;
			Scene::SetActiveScene(nullptr);
		}
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		using namespace Vision;

		if (deltaTime >= 0.5f)
		{
			deltaTime = 0.5f;
		}

		if (m_SceneViewPanel.IsIntractable())
		{
			bool control = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);
			bool shift   = Input::IsKeyDown(Key::LeftShift)   || Input::IsKeyDown(Key::RightShift);

			if (!control && !shift)
			{
				m_CameraController->OnUpdate(deltaTime);
			}
		}

		m_SceneFrameBuffer->Bind();

		RenderCommand::Clear(RendererAPI::ColorBuffer);

		Renderer2D::BeginScene(m_CameraController->GetCameraTransform(), m_CameraController->Camera, m_SpriteShader);

		Scene* scene = Scene::GetActiveScene();

		if (scene)
		{
			Renderer2D::DrawTexture(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f,
									glm::vec2(100.0f, 100.0f),
									m_CheckboardTexture,
									glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
									100.0f);

			scene->EachGroup<TransformComponent, SpriteRendererComponent>([](auto& transform, auto& sprite)
			{
				Renderer2D::DrawSprite(transform.Position, transform.Rotation.z, transform.Scale, sprite);
			});
		}

		Renderer2D::EndScene();

		m_SceneFrameBuffer->UnBind();
 	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VN_BIND_EVENT_FN(EditorLayer::OnKeyPressed));

		if (m_SceneViewPanel.IsIntractable())
		{
			e.Handled = false;
			m_CameraController->OnEvent(e);
		}
		else
		{
			e.Handled = true;
		}
	}

	void EditorLayer::OnImGuiRender()
	{
		using namespace Vision;

		Window& window = Application::Get().GetWindow();
		Scene* scene = Scene::GetActiveScene();

		if (m_SceneViewPanel.IsViewportResized())
		{
			glm::vec2 viewportSize = m_SceneViewPanel.GetViewportSize();
			m_CameraController->Resize((uint32)viewportSize.x, (uint32)viewportSize.y);
		}

		m_Menubar.OnImGuiRender();
		m_SceneHierarchyPanel.OnImGuiRender();
		// m_GameViewPanel.OnImGuiRender();
		m_InspectorPanel.OnImGuiRender();
		// m_ConsolePanel.OnImGuiRender();
		m_SceneViewPanel.OnImGuiRender();
		m_Dialog.OnImGuiRender();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
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
					Dialog::Open(DialogType::CreateScene);
				}
			}
			break;

			case Key::S:
			{
				if (control && shift)
				{
					Scene* scene = Scene::GetActiveScene();

					if (scene)
					{
						SaveSceneAs(scene);
					}
				}
				else if (control)
				{
					Scene* scene = Scene::GetActiveScene();

					if (scene)
					{
						SaveScene(scene);
					}
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

					if (scene)
					{
						CloseScene(scene);
					}
				}
			}
			break;

			case Key::Delete:
			{
				Scene* scene = Scene::GetActiveScene();

				if (scene && m_SceneHierarchyPanel.IsInteractable())
				{
					if (!scene->EditorState.SelectedEntityTag.empty())
					{
						scene->FreeEntity(scene->EditorState.SelectedEntityTag);
						scene->EditorState.SelectedEntityTag = "";
					}
				}
			}

			case Key::R:
			{
				m_SpriteShader->Reload();
			}
		}

		return false;
	}

	void EditorLayer::LoadSettings()
	{
		using namespace Vision;

		m_Window->SetTitle("Eagle Eye");

		std::ifstream ifs("Settings.json");

		std::string line;
		std::string contents;

		while (std::getline(ifs, line))
		{
			contents += line;
		}

		m_Settings.Parse(contents.c_str(), contents.length());

		using TDS = TextDeserializer;

		int32  windowX      = TDS::DeserializeInt32(m_Settings, "WindowX");
		int32  windowY      = TDS::DeserializeInt32(m_Settings, "WindowY");
		uint32 windowWidth  = TDS::DeserializeUint32(m_Settings, "WindowWidth");
		uint32 windowHeight = TDS::DeserializeUint32(m_Settings, "WindowHeight");
		bool   maximized    = TDS::DeserializeBool(m_Settings, "Maximized");

		m_Window->SetPosition(windowX, windowY);
		m_Window->SetSize(windowWidth, windowHeight);

		if (maximized)
		{
			m_Window->Maximize();
		}
	}

	void EditorLayer::SaveSettings()
	{
		using namespace Vision;

		rapidjson::StringBuffer s;
		Writer writer(s);

		writer.StartObject();

		const WindowData& windowData = m_Window->GetData();

		using TS = TextSerializer;

		TS::SerializeInt32(writer, "WindowX", windowData.WindowX);
		TS::SerializeInt32(writer, "WindowY", windowData.WindowY);
		TS::SerializeUint32(writer, "WindowWidth", windowData.Width);
		TS::SerializeUint32(writer, "WindowHeight", windowData.Height);
		TS::SerializeBool(writer, "Maximized", windowData.Maximized);

		TS::SerializeString(writer, "Last Scene Path", m_LastScenePath);

		// @Clean Up: Should be save with the scene ...
		TS::SerializeFloat(writer, "CameraZoomLevel", m_CameraController->Camera.Size);

		writer.EndObject();

		std::ofstream ofs("Settings.json");

		if (ofs.is_open())
		{
			ofs << s.GetString();
			VN_CORE_INFO("Saving Editor Settings");
		}
		else
		{
			VN_CORE_INFO("Can't OpenFile: Settings.json");
		}
	}

	void EditorLayer::NewScene(const std::string& filepath, uint32 maxEntityCount)
	{
		Scene* scene = Scene::GetActiveScene();

		if (scene)
		{
			TextSerializer::SerializeScene(scene->Path, scene);
			delete scene;
		}

		Scene* newScene = new Scene();
		newScene->Path = filepath;
		newScene->Name = FileSystem::GetFileName(filepath, false);
		newScene->MaxEntityCount = maxEntityCount;

		Scene::SetActiveScene(newScene);
		SaveScene(newScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialog::OpenFile("Vision Scene (*.vscene)", { "vscene" });

		if (!filepath.empty())
		{
			Scene* scene = Scene::GetActiveScene();

			if (scene)
			{
				SaveScene(scene);
				delete scene;
			}

			Scene* newScene = new Scene();
			newScene->Path = filepath;
			newScene->Name = FileSystem::GetFileName(filepath, false);

			Scene::SetActiveScene(newScene);
			TextDeserializer::DeserializeScene(filepath, newScene);
		}
	}

	void EditorLayer::SaveSceneAs(Scene* scene)
	{
		std::string filepath = FileDialog::SaveFile("Vision Scene (*.vscene)", "vscene");

		if (!filepath.empty())
		{
			SaveScene(scene);
		}
	}

	void EditorLayer::SaveScene(Scene* scene)
	{
		TextSerializer::SerializeScene(scene->Path, scene);
	}

	void EditorLayer::CloseScene(Scene* scene)
	{
		SaveScene(scene);
		delete scene;
		Scene::SetActiveScene(nullptr);
	}
}