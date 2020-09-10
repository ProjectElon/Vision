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
		, m_WhiteColor(1.0f, 1.0f, 1.0f, 1.0f)
	{
		LoadSettings();
		m_MainScene = Vision::CreateScope<Scene>("MainScene");
		Scene::SetActiveScene(m_MainScene.get());
	}

	EditorLayer::~EditorLayer()
	{
		SaveSettings();
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
		m_PlayerTexture = Texture2D::CreateFromFile(texturePath + "brick_red.png", transparent);
		
		Entity camera0 = m_MainScene->CreateEntity("Camera0", TransformComponent{}, OrthographicCameraComponent{});
		m_MainScene->SetActiveCamera(camera0);

		Entity entity0 = m_MainScene->CreateEntity("Entity0", TransformComponent{}, SpriteComponent{});
		auto& sc = entity0.GetComponent<SpriteComponent>();
		sc.Texture = m_PlayerTexture;

		InspectComponent<TagComponent>();
		InspectComponent<TransformComponent>();
		InspectComponent<OrthographicCameraComponent>();
		InspectComponent<SpriteComponent>();
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

			static bool CWasDown = false;

			if (Input::IsKeyDown(VN_KEY_C))
			{
				CWasDown = true;
			}

			if (Input::IsKeyUp(VN_KEY_C) && CWasDown)
			{
				m_UseMainGameCamera = !m_UseMainGameCamera;
				CWasDown = false;
			}
		}

		Scene& scene = Scene::GetActiveScene();
		Entity activeCamera = scene.GetActiveCamera();

		const auto& cameraTransform = activeCamera.GetComponent<TransformComponent>().Transform;
		const auto& cameraComponent = activeCamera.GetComponent<OrthographicCameraComponent>();

		m_SceneFrameBuffer->Bind();

		RenderCommand::Clear(RendererAPI::ColorBuffer);
		
		if (m_UseMainGameCamera)
		{
			Renderer2D::BeginScene(cameraTransform, cameraComponent, m_SpriteShader);
		}
		else
		{
			Renderer2D::BeginScene(m_CameraController->GetCameraTransform(), m_CameraController->GetCamera(), m_SpriteShader);
		}

		Renderer2D::DrawTexture(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, 
								glm::vec2(100.0f, 100.0f), 
								m_CheckboardTexture, 
								glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
								100.0f);

		m_MainScene->Each<TransformComponent, SpriteComponent>([](auto& components, Entity entity)
		{
			auto& [transform, sprite] = components;
			Renderer2D::DrawSprite(transform.Transform, sprite);
		});

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
		auto& scene = Scene::GetActiveScene();

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
			ImGui::Begin("Scene Viewport");
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
				
				float aspectRatio = currentViewportSize.x / currentViewportSize.y;

				scene.Each<OrthographicCameraComponent>([&](OrthographicCameraComponent& camera, Entity entity)
				{
					if (!camera.Static)
					{
						camera.AspectRatio = aspectRatio;
						camera.Projection = glm::ortho(-camera.AspectRatio * camera.Size,
														camera.AspectRatio * camera.Size,
													   -camera.Size,
														camera.Size);
					}
				});

				/*
				scene.for_each<PerspectiveCameraComponent>([&](PerspectiveCameraComponent& camera, Entity entity)
				{
					if (!camera.Static)
					{
					camera.AspectRatio = aspectRatio;
					camera.Projection = glm::perspective(camera.FieldOfView, 
														 camera.AspectRatio, 
														 camera.Near, 
														 camera.Far);
					}
				});
				*/
			}

			// Scene Panel
			{
				ImGui::Begin("Scene Hierarchy");

				if (ImGui::TreeNodeEx(scene.GetName().c_str(), ImGuiTreeNodeFlags_DefaultOpen | ImGuiTreeNodeFlags_FramePadding))
				{
					scene.Each([&](Entity entity)
					{
						auto& tag = entity.GetComponent<TagComponent>();
						const char* entityName = tag.Tag.c_str();

						ImGuiTreeNodeFlags flags = ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_FramePadding;

						if (entity == m_SelectedEntity)
						{
							flags |= ImGuiTreeNodeFlags_Selected;
						}

						bool expanded = ImGui::TreeNodeEx((void*)(intptr_t)entity.GetHandle(), flags, entityName);
						
						if (ImGui::IsItemClicked() || ImGui::IsItemToggledOpen())
						{
							m_SelectedEntity = entity;
						}
						
						if (expanded) ImGui::TreePop();
						
					});

					ImGui::TreePop();
				}

				ImGui::End();
			}
		}


		// Inspector
		{
			ImGui::Begin("Inspector");
			
			if (m_SelectedEntity.Isvalid())
			{
				const auto& storage = m_SelectedEntity.GetStorage();
				
				for (auto& [componentID, componentIndex] : storage)
				{
					void* component = m_SelectedEntity.GetComponent(componentID, componentIndex);
					m_ComponentInspectors[componentID](component);
				}
			}

			ImGui::End();
		}
		
		// Console
		{
			ImGui::Begin("Console");
			ImGui::Text("%.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
			ImGui::End();
		}

		ImGui::End();
	}

	void EditorLayer::LoadSettings()
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

		writer.Key("VSync");
		writer.Bool(windowData.VSync);

		writer.Key("CameraZoomLevel");
		writer.Uint(m_CameraController->GetCamera().Size);

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
}