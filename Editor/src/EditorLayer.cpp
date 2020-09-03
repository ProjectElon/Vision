#include "EditorLayer.h"
#include <imgui.h>
#include <rapidjson/prettywriter.h>
#include <rapidjson/stringbuffer.h>
#include <fstream>

namespace Vision
{
	EditorLayer::EditorLayer()
		: Layer("Editor")
		, m_WhiteColor(1.0f, 1.0f, 1.0f, 1.0f)
	{
		using namespace Vision;

		m_Window = &Application::Get().GetWindow();
		m_Window->SetTitle("Eagle Eye");

		std::ifstream ifs("Settings.json");
		
		std::string line;
		std::string contents;

		while (std::getline(ifs, line))
		{
			contents += line;
		}

		m_Settings.Parse(contents.c_str());

		int32 windowX = m_Settings["WindowX"].GetInt();
		int32 windowY = m_Settings["WindowY"].GetInt();
		uint32 windowWidth = m_Settings["WindowWidth"].GetUint();
		uint32 windowHeight = m_Settings["WindowHeight"].GetUint();
		bool maximized = m_Settings["Maximized"].GetBool();
		m_IsVSyncEnabled = m_Settings["VSync"].GetBool();
		m_LastIsVSyncEnabled = m_IsVSyncEnabled;

		const rapidjson::Value& clearColor = m_Settings["ClearColor"];
		m_ClearColor = glm::vec4(clearColor[0].GetFloat(),
			clearColor[1].GetFloat(),
			clearColor[2].GetFloat(),
			clearColor[3].GetFloat());

		m_Window->SetPosition(windowX, windowY);
		m_Window->SetSize(windowWidth, windowHeight);
		m_Window->SetVSync(m_IsVSyncEnabled);

		if (maximized)
		{
			m_Window->Maximize();
		}

		struct Transform
		{
			glm::mat4 Transform;
		};

		struct Velocity
		{
			float Amount;
			glm::vec3 Direction;
		};

		struct Position
		{
			uint32 x;
			uint32 y;
		};

		Scene scene;
		
		Entity entity = scene.CreateEntity("Entity");
		entity.AddComponents(Position{ 1, 2 }, Velocity{ 5, glm::vec3(1.0f, 2.0f, 3.0f) });
		
		Entity entity0 = scene.CreateEntity("Entity0");
		entity0.AddComponents(Position{ 3, 4 }, Velocity{ 5, glm::vec3(1.0f, 2.0f, 3.0f) });

		Entity entity1 = scene.CreateEntity("Entity1");
		entity1.AddComponents(Position{ 5, 6 }, Velocity{ 5, glm::vec3(1.0f, 2.0f, 3.0f) });

		scene.for_each<TagComponent>([](auto& component, Entity entity) -> void
		{
			VN_CORE_TRACE("Entity: {0}", component.Tag);
		});

		scene.for_each<Position, Velocity>([](auto& components, Entity entity)
		{
			auto [position, velocity] = components;
			position.x = 5;
			VN_CORE_TRACE("({0}, {1})", position.x, position.y);
			VN_CORE_TRACE("({0}, ({1}, {2}, {3}))", velocity.Amount, velocity.Direction.x, velocity.Direction.y, velocity.Direction.z);
		});

		scene.for_each([](Entity entity)
		{
			VN_CORE_TRACE(entity.GetComponent<TagComponent>().Tag);
		});

		VN_CORE_INFO("({0})", entity0.HasComponents<Position, Velocity>());

		auto[position, velocity] = entity.GetComponents<Position, Velocity>();

		VN_CORE_TRACE("({0}, {1})", position.x, position.y);
		VN_CORE_TRACE("({0}, ({1}, {2}, {3}))", velocity.Amount, velocity.Direction.x, velocity.Direction.y, velocity.Direction.z);

		auto& po = entity.GetComponent<Position>();

		VN_CORE_TRACE("({0}, {1})", po.x, po.y);

		auto p = entity.RemoveComponent<Position>();

		VN_CORE_TRACE("({0}, {1})", p.x, p.y);
		
		auto [v, t] = entity.RemoveComponents<Velocity, TagComponent>();
		VN_CORE_TRACE("({0}, ({1}, {2}, {3}))", v.Amount, v.Direction.x, v.Direction.y, v.Direction.z);
		VN_CORE_TRACE("({0})", t.Tag);
	}

	EditorLayer::~EditorLayer()
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

		writer.Key("VSync");
		writer.Bool(windowData.VSync);

		writer.Key("CameraZoomLevel");
		writer.Uint(m_CameraController->GetZoomLevel());

		writer.Key("ClearColor");
		writer.StartArray();
		writer.Double(m_ClearColor.r);
		writer.Double(m_ClearColor.g);
		writer.Double(m_ClearColor.b);
		writer.Double(m_ClearColor.a);
		writer.EndArray();

		writer.EndObject();

		std::ofstream ofs("Settings.json");
		ofs << s.GetString();
	}

	void EditorLayer::OnAttach()
	{
		using namespace Vision;

		FrameBufferProps props;
		props.Width = 800;
		props.Height = 600;
		m_SceneFrameBuffer = FrameBuffer::Create(props);

		float aspectRatio = (float)props.Width / (float)props.Height;
		m_CameraController = CreateScope<OrthographicCameraController>(aspectRatio, m_Settings["CameraZoomLevel"].GetFloat());

		m_SpriteShader = Shader::CreateFromFile("Assets/Shaders/Sprite.glsl");

		TextureProps tiled;

		tiled.WrapX = WrapMode::Repeat;
		tiled.WrapY = WrapMode::Repeat;
		tiled.Filter = FilterMode::Point;

		TextureProps transparent;
		transparent.WrapX = WrapMode::ClampToEdge;
		transparent.WrapY = WrapMode::ClampToEdge;
		transparent.Filter = FilterMode::Bilinear;

		std::string texturePath = "Assets/Textures/";

		m_CheckboardTexture = Texture2D::CreateFromFile(texturePath + "Checkerboard.png", tiled);

		m_CheckerboardSprite = CreateRef<Sprite>("Checkerboard", m_CheckboardTexture);
		m_CheckerboardSprite->TopRightUV = glm::vec2(50.0f, 50.0f);
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

		if (m_IsViewportHovered && m_IsViewportFocused)
		{
			m_CameraController->OnUpdate(deltaTime);

			static bool RWasDown = false;

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

		m_SceneFrameBuffer->Bind();

		RenderCommand::Clear(RendererAPI::ColorBuffer);

		Renderer2D::BeginScene(m_CameraController->GetCamera(), m_SpriteShader);

		Renderer2D::DrawSprite(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec2(100.0f, 100.0f), m_CheckerboardSprite);

		Renderer2D::EndScene();

		m_SceneFrameBuffer->UnBind();
	}

	void EditorLayer::OnEvent(Vision::Event& e)
	{
		if (m_IsViewportFocused && m_IsViewportHovered)
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

		// Menu Bar
		{
			ImGui::BeginMainMenuBar();

			if (ImGui::BeginMenu("File", true))
			{
				if (ImGui::MenuItem("Add Sprite", nullptr, nullptr))
				{
					VN_CORE_INFO("Adding Sprite To Editor ...");
				}

				ImGui::EndMenu();
			}

			ImGui::EndMainMenuBar();
		}

		// Graphics
		{
			ImGui::Begin("Graphics Settings");

			ImGui::ColorEdit4("Clear Color", &m_ClearColor.r);

			if (m_LastClearColor != m_ClearColor)
			{
				RenderCommand::SetClearColor(m_ClearColor);
				m_LastClearColor = m_ClearColor;
			}

			ImGui::Checkbox("VSync", &m_IsVSyncEnabled);

			if (m_IsVSyncEnabled != m_LastIsVSyncEnabled)
			{
				window.SetVSync(m_IsVSyncEnabled);
				m_LastIsVSyncEnabled = m_IsVSyncEnabled;
			}

			ImGui::End();
		}

		// Viewport
		{
			ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
			ImGui::Begin("Viewport");
			ImGui::PopStyleVar();

			m_IsViewportFocused = ImGui::IsWindowFocused();
			m_IsViewportHovered = ImGui::IsWindowHovered();

			ImVec2 currentViewportSize = ImGui::GetContentRegionAvail();

			uint32 texture = m_SceneFrameBuffer->GetColorAttachmentRendererId();
			ImGui::Image((void*)(intptr_t)texture, { m_ViewportSize.x, m_ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));

			if (m_ViewportSize != glm::vec2(currentViewportSize.x, currentViewportSize.y) &&
				currentViewportSize.x > 0.0f &&
				currentViewportSize.y > 0.0f)
			{
				m_CameraController->Resize((uint32)currentViewportSize.x, (uint32)currentViewportSize.y);
				m_SceneFrameBuffer->Resize((uint32)currentViewportSize.x, (uint32)currentViewportSize.y);
				m_ViewportSize = { currentViewportSize.x, currentViewportSize.y };
			}
		}
		
		// Console
		{
			ImGui::Begin("Console");
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::End();
	}
}