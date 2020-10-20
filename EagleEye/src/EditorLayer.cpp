#include "EditorLayer.h"
#include <imgui.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>
#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	EditorLayer::EditorLayer()
		: Layer("Editor")
	{
		m_Window = &Application::Get().GetWindow();
		m_Window->SetVSync(true);

		m_MainScene = Vision::CreateScope<Scene>("MainScene", 1000);
		Scene::SetActiveScene(m_MainScene.get());
		
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
		frameBufferProps.Width  = 800;
		frameBufferProps.Height = 600;

		m_SceneFrameBuffer = FrameBuffer::Create(frameBufferProps);
		m_GameFrameBuffer  = FrameBuffer::Create(frameBufferProps);

		m_SceneViewPanel.SetFrameBuffer(m_SceneFrameBuffer.get());
		m_GameViewPanel.SetFrameBuffer(m_GameFrameBuffer.get());

		float aspectRatio  = (float)frameBufferProps.Width / (float)frameBufferProps.Height;
		m_CameraController = CreateScope<OrthographicCameraController>(aspectRatio, m_Settings["CameraZoomLevel"].GetFloat());

		m_SpriteShader = Shader::CreateFromFile("Assets/Shaders/Sprite.glsl");

		TextureProps tiled;

		tiled.WrapX  = WrapMode::Repeat;
		tiled.WrapY  = WrapMode::Repeat;
		tiled.FilterMode = FilterMode::Point;

		TextureProps transparent;
		transparent.WrapX  = WrapMode::ClampToEdge;
		transparent.WrapY  = WrapMode::ClampToEdge;
		transparent.FilterMode = FilterMode::Bilinear;

		std::string texturePath = "Assets/Textures/";
		
		m_CheckboardTexture = Texture2D::CreateFromFile(texturePath + "Checkerboard.png", tiled);
		m_PlayerTexture = Texture2D::CreateFromFile(texturePath + "brick_red.png", transparent);
		
		Entity camera0 = m_MainScene->CreateEntity("Camera0", TransformComponent{}, OrthographicCameraComponent{});
		m_MainScene->ActiveCameraTag = "Camera0";

		Entity camera1 = m_MainScene->CreateEntity("Camera1", TransformComponent{}, OrthographicCameraComponent{});
		
		Entity entity0 = m_MainScene->CreateEntity("Entity0", TransformComponent{}, SpriteRendererComponent{});
		auto& sc = m_MainScene->GetComponent<SpriteRendererComponent>(entity0);
		sc.Texture = m_PlayerTexture;
	}

	void EditorLayer::OnDetach()
	{
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
			m_CameraController->OnUpdate(deltaTime);

			static bool RWasDown = false;
			static bool SpaceWasDown = false;

			if (Input::IsKeyDown(VN_KEY_R))
			{
				RWasDown = true;
			}

			if (Input::IsKeyUp(VN_KEY_R) && RWasDown)
			{
				m_SpriteShader->Reload();
				RWasDown = false;
			}
		}

		Scene& scene = Scene::GetActiveScene();	

		m_SceneFrameBuffer->Bind();

		RenderCommand::Clear(RendererAPI::ColorBuffer);

		Renderer2D::BeginScene(m_CameraController->GetCameraTransform(), m_CameraController->Camera, m_SpriteShader);

		Renderer2D::DrawTexture(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f,
								glm::vec2(100.0f, 100.0f),
								m_CheckboardTexture,
								glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
								100.0f);

		scene.EachGroup<TransformComponent, SpriteRendererComponent>([](auto& transform, auto& sprite)
		{
			Renderer2D::DrawSprite(transform.Position, transform.Rotation.z, transform.Scale, sprite);
		});

		Renderer2D::EndScene();

		m_SceneFrameBuffer->UnBind();
 	}

	void EditorLayer::OnEvent(Event& e)
	{
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

		auto& window = Application::Get().GetWindow();
		Scene& scene = Scene::GetActiveScene();

		if (m_SceneViewPanel.IsViewportResized())
		{
			auto viewportSize = m_SceneViewPanel.GetViewportSize();
			m_CameraController->Resize((uint32)viewportSize.x, (uint32)viewportSize.y);
		}
		
		m_Menubar.OnImGuiRender();
		m_SceneHierarchyPanel.OnImGuiRender();
		// m_GameViewPanel.OnImGuiRender();
		m_SceneViewPanel.OnImGuiRender();
		m_InspectorPanel.OnImGuiRender();
		m_ConsolePanel.OnImGuiRender();

		static bool deleteWasDown = false;

		if (m_SceneHierarchyPanel.IsInteractable())
		{
			if (Input::IsKeyDown(VN_KEY_DELETE))
			{
				deleteWasDown = true;
			}

			if (Input::IsKeyUp(VN_KEY_DELETE) && deleteWasDown)
			{
				deleteWasDown = false;
				scene.FreeEntity(scene.EditorState.SeleteEntityTag);
				scene.EditorState.SeleteEntityTag = "";
			}
		}
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

		m_Settings.Parse(contents.c_str());

		int32  windowX      = m_Settings["WindowX"].GetInt();
		int32  windowY      = m_Settings["WindowY"].GetInt();
		uint32 windowWidth  = m_Settings["WindowWidth"].GetUint();
		uint32 windowHeight = m_Settings["WindowHeight"].GetUint();
		bool   maximized    = m_Settings["Maximized"].GetBool();
		
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
		rapidjson::PrettyWriter<rapidjson::StringBuffer> writer(s);
		writer.StartObject();

		const WindowData& windowData = m_Window->GetData();

		writer.Key("WindowX");
		writer.Int(windowData.WindowX);

		writer.Key("WindowY");
		writer.Int(windowData.WindowY);

		writer.Key("WindowWidth");
		writer.Uint(windowData.Width);

		writer.Key("WindowHeight");
		writer.Uint(windowData.Height);

		writer.Key("Maximized");
		writer.Bool(windowData.Maximized);

		writer.Key("CameraZoomLevel");
		writer.Double(m_CameraController->Camera.Size);

		writer.EndObject();

		std::ofstream ofs("Settings.json");
		ofs << s.GetString();
	}
}