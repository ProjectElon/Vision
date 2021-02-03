#include "EditorLayer.h"

#include <fstream>
#include <sstream>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>

namespace Vision
{
	EditorLayer::EditorLayer()
		: Layer("Editor")
	{
		Window& window = Application::Get().GetWindow();
		SetWindowTitle(&window, "Eagle Eye");
		Renderer::SetVSync(true);
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		LoadSettings();

		using namespace Vision;

		FrameBufferAttachmentSpecification SceneBufferSpecification;
		SceneBufferSpecification.Attachments = { FrameBufferTextureFormat::RGBA8, FrameBufferTextureFormat::RedInteger, FrameBufferTextureFormat::Depth24Stencil8 };

		InitFrameBuffer(&m_SceneFrameBuffer, SceneBufferSpecification, 800, 600);
		m_SceneViewPanel.SetFrameBuffer(&m_SceneFrameBuffer);

		EditorState& editorState = Scene::EditorState;
		m_SceneCamera = &editorState.SceneCamera;
		ResizeOrthographisCamera(m_SceneCamera, 800, 600);

		m_SpriteShader = Assets::RequestAsset("Assets/Shaders/Sprite.glsl");
		m_FontShader = Assets::RequestAsset("Assets/Shaders/Font.glsl");
		m_CheckboardTexture = Assets::RequestAsset("Assets/Textures/Checkerboard.png");

		m_FiraCodeFont = Assets::RequestAsset("Assets/Fonts/FiraCode-Regular.ttf");
		BitmapFont* firaCodeFont = Assets::GetBitmapFont(m_FiraCodeFont);
		SetFontSize(firaCodeFont, 72);

		WatchDirectory("Assets", VnBindWatcherFn(EditorLayer::OnFileChanged));

		m_Menubar.SetEditor(this);
		m_Dialog.SetEditor(this);
	}

	void EditorLayer::OnDetach()
	{
		SaveSettings();

		if (m_ActiveScene)
		{
			SaveActiveScene();
		}
	}

	void EditorLayer::OnFileChanged(FileWatcherAction action,
									std::string filepath,
									std::string oldpath)
	{
		std::string extension = FileSystem::GetFileExtension(filepath, false);

		switch (action)
		{
			case Vision::FileWatcherAction::FileAdded:
			{
			}
			break;

			case Vision::FileWatcherAction::FileRemoved:
			{
			}
			break;

			case Vision::FileWatcherAction::FileModified:
			{
				std::string cwd = FileSystem::GetCurrentWorkingDirectory();
				std::string fileRelativePath = filepath.substr(cwd.length() + 1);

				bool isFile = FileSystem::FileExists(fileRelativePath);

				if (isFile)
				{
					Assets::ReloadAsset(fileRelativePath);
				}
			}
			break;

			case Vision::FileWatcherAction::FileRenamed:
			{
			}
			break;
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
			bool isControlDown = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);
			bool isShiftDown   = Input::IsKeyDown(Key::LeftShift)   || Input::IsKeyDown(Key::RightShift);

			if (m_ActiveScene && !isControlDown && !isShiftDown)
			{
				UpdateOrthographicCamera(m_SceneCamera, deltaTime);
			}
		}

		BindFrameBuffer(&m_SceneFrameBuffer);
		Renderer::Clear(ClearColorBuffer);

		if (m_ActiveScene)
		{
			Renderer2D::BeginScene(glm::translate(glm::mat4(1.0f), m_SceneCamera->Position),
								   m_SceneCamera->Projection,
								   Assets::GetShader(m_SpriteShader));

			Scene* scene = Assets::GetScene(m_ActiveScene);

			Renderer2D::DrawTexture(glm::vec3(0.0f, 0.0f, 0.0f),
			                        0.0f,
									glm::vec2(100.0f, 100.0f),
									Assets::GetTexture(m_CheckboardTexture),
									glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
									glm::vec2(0.0f, 0.0f),
									glm::vec2(1.0f, 1.0f) * 100.0f);

			scene->EachGroup<TransformComponent, SpriteRendererComponent>([](Entity entity, auto& transform, auto& sprite)
			{
				Renderer2D::DrawTexture(transform.Position,
										transform.Rotation.z,
										transform.Scale,
										Assets::GetTexture(sprite.Texture),
										sprite.Color,
										sprite.BottomLeftUV,
										sprite.TopRightUV,
										entity);
			});

			Renderer2D::EndScene();

			glm::mat4 ortho = glm::ortho(0.0f,
			                             (float32)m_SceneFrameBuffer.Width,
									     (float32)m_SceneFrameBuffer.Height,
									     0.0f);

			Renderer2D::BeginScene(glm::translate(glm::mat4(1.0f), m_SceneCamera->Position),
								   ortho,
								   Assets::GetShader(m_FontShader));

			Renderer2D::DrawString(Assets::GetBitmapFont(m_FiraCodeFont),
								   "Vision Engine!",
									glm::vec2(100.0f, 100.0f),
			                        glm::vec4(0.1f, 0.1f, 0.1f, 1.0f));

			Renderer2D::EndScene();

			const glm::vec2* bounds = m_SceneViewPanel.GetViewportBounds();
			glm::vec2 minBound = bounds[0];
			glm::vec2 maxBound = bounds[1];

			auto [mx, my] = ImGui::GetMousePos();

			mx -= minBound.x;
			my -= minBound.y;

			glm::vec2 viewportSize = maxBound - minBound;

			my = viewportSize.y - my;

			int32 mouseX = (int32)mx;
			int32 mouseY = (int32)my;

			EditorState& editorState = Scene::EditorState;

			if (mouseX >= 0 && mouseX < (int32)viewportSize.x && mouseY >= 0 && mouseY < (int32)viewportSize.y)
			{
				if (Input::IsMouseButtonDown(Mouse::ButtonLeft))
				{
					int32 pixel = ReadPixel(&m_SceneFrameBuffer, 1, mouseX, mouseY);
					
					if (pixel > 0)
					{
						bool shouldSelectEntity = false;

						if (editorState.SelectedEntityTag.empty())
						{
							shouldSelectEntity = true;
						}
						else
						{
							auto tagIt = scene->Tags.find(editorState.SelectedEntityTag);
							uint32 seletedEntity = tagIt->second;

							if (seletedEntity != pixel)
							{
								shouldSelectEntity = true;
							}
						}

						if (shouldSelectEntity)
						{
							auto& tagComponent = scene->GetComponent<TagComponent>(pixel);
							editorState.SelectedEntityTag = tagComponent.Tag;
						}
					}
				}
			}
		}

		UnBindFrameBuffer(&m_SceneFrameBuffer);
 	}

	void EditorLayer::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<KeyPressedEvent>(VnBindEventFn(EditorLayer::OnKeyPressed));
		dispatcher.Dispatch<MouseWheelScrolledEvent>(VnBindEventFn(EditorLayer::OnMouseWheelScrolled));
	}

	void EditorLayer::OnImGuiRender()
	{
		using namespace Vision;

		if (m_SceneViewPanel.IsViewportResized())
		{
			glm::vec2 viewportSize = m_SceneViewPanel.GetViewportSize();

			if (m_ActiveScene)
			{
				ResizeOrthographisCamera(m_SceneCamera,
										 (uint32)viewportSize.x,
										 (uint32)viewportSize.y);
			}
		}

		m_Menubar.OnImGuiRender();
		m_SceneHierarchyPanel.OnImGuiRender();
		m_InspectorPanel.OnImGuiRender();
		m_SceneViewPanel.OnImGuiRender();
		m_Dialog.OnImGuiRender();
	}

	bool EditorLayer::OnKeyPressed(KeyPressedEvent& e)
	{
		if (e.GetRepeatCount() > 0)
		{
			return false;
		}

		bool isControlDown = Input::IsKeyDown(Key::LeftControl) || Input::IsKeyDown(Key::RightControl);
		bool isShiftDown = Input::IsKeyDown(Key::LeftShift) || Input::IsKeyDown(Key::RightShift);

		switch (e.GetKeyCode())
		{
			case Key::N:
			{
				if (isControlDown && isShiftDown)
				{
					Dialog::Open(DialogType::CreateScene);
				}
			}
			break;

			case Key::S:
			{
				if (isControlDown && isShiftDown)
				{
					if (m_ActiveScene)
					{
						SaveActiveSceneAs();
					}
				}
				else if (isControlDown)
				{
					if (m_ActiveScene)
					{
						SaveActiveScene();
					}
				}
			}
			break;

			case Key::O:
			{
				if (isControlDown && isShiftDown)
				{
					OpenScene();
				}
			}
			break;

			case Key::E:
			{
				if (isControlDown)
				{
					if (m_ActiveScene)
					{
						CloseActiveScene();
					}
				}
			}
			break;

			case Key::Delete:
			{
				if (m_ActiveScene && m_SceneHierarchyPanel.IsInteractable())
				{
					Scene* scene = Assets::GetScene(m_ActiveScene);

					if (!scene->EditorState.SelectedEntityTag.empty())
					{
						scene->FreeEntity(scene->EditorState.SelectedEntityTag);
						scene->EditorState.SelectedEntityTag = "";
					}
				}
			}
			break;

			case Key::Escape:
			{
				Application& app = Application::Get();
				app.Close();
			}
			break;
		}

		return false;
	}

	bool EditorLayer::OnMouseWheelScrolled(MouseWheelScrolledEvent& e)
	{
		if (m_SceneViewPanel.IsIntractable())
		{
			UpdateOrthographicCameraSize(m_SceneCamera, -e.GetYOffset());
		}

		return false;
	}

	void EditorLayer::LoadSettings()
	{
		using namespace Vision;

		FileStream handle = File::Open("Settings.vars", FileMode::Read);

		std::string line;
		std::string contents;

		while (File::ReadLine(handle, line))
		{
			contents += line + " ";
		}

		std::string prop;
		std::stringstream stringStream(contents);

		int windowX;
		int windowY;
		int windowWidth;
		int windowHeight;
		bool maximized;
		std::string scenePath;

		stringStream >> prop >> windowX;
		stringStream >> prop >> windowY;
		stringStream >> prop >> windowWidth;
		stringStream >> prop >> windowHeight;
		stringStream >> prop >> maximized;
		stringStream >> prop >> scenePath; // @(Harlequin): not right because the path may contain spaces

		Window& window = Application::Get().GetWindow();

		SetWindowPosition(&window, windowX, windowY);
		SetWindowSize(&window, windowWidth, windowHeight);

		if (maximized)
		{
			MaximizeWindow(&window);
		}

		if (scenePath != "none")
		{
			m_ActiveScene = Assets::RequestAsset(scenePath);
			m_SceneHierarchyPanel.SetActiveScene(m_ActiveScene);
			m_InspectorPanel.SetActiveScene(m_ActiveScene);
		}

		File::Close(handle);
	}

	void EditorLayer::SaveSettings()
	{
		using namespace Vision;

		FileStream handle = File::Open("Settings.vars", FileMode::Write);

		Window& window = Application::Get().GetWindow();

		std::stringstream stringstream;
		stringstream << "WindowX " << window.WindowX << "\n";
		stringstream << "WindowY " << window.WindowY << "\n";
		stringstream << "WindowWidth " << window.Width << "\n";
		stringstream << "WindowHeight " << window.Height << "\n";
		stringstream << "WindowMaximized " << window.Maximized << "\n";

		std::string activeScenePath = "none";

		if (m_ActiveScene)
		{
			const Asset& activeSceneAsset = Assets::GetAsset(m_ActiveScene);
			activeScenePath = activeSceneAsset.Path;
		}

		stringstream << "ScenePath " << activeScenePath << "\n";

		File::WriteContents(handle, stringstream.str());
		File::Close(handle);
	}

	void EditorLayer::NewScene(const std::string& filepath, uint32 maxEntityCount)
	{
		if (m_ActiveScene)
		{
			SaveActiveScene();
			Assets::ReleaseAsset(m_ActiveScene);
		}

		InitOrthographicCamera(m_SceneCamera,
							   m_SceneCamera->AspectRatio);

		Scene* tempScene = new Scene;
		CreateScene(tempScene, maxEntityCount);
		SerializeScene(filepath, tempScene);

		DestroyScene(tempScene);
		delete tempScene;

		m_ActiveScene = Assets::RequestAsset(filepath);
		m_SceneHierarchyPanel.SetActiveScene(m_ActiveScene);
		m_InspectorPanel.SetActiveScene(m_ActiveScene);
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialog::OpenFile("Vision Scene (*.vscene)", { "vscene" });

		if (!filepath.empty())
		{
			if (m_ActiveScene)
			{
				SaveActiveScene();
				Assets::ReleaseAsset(m_ActiveScene);
			}

			m_ActiveScene = Assets::RequestAsset(filepath);
			m_SceneHierarchyPanel.SetActiveScene(m_ActiveScene);
			m_InspectorPanel.SetActiveScene(m_ActiveScene);
		}
	}

	void EditorLayer::SaveActiveSceneAs()
	{
		std::string filepath = FileDialog::SaveFile("Vision Scene (*.vscene)", "vscene");

		if (!filepath.empty())
		{
			Scene* activeScene = Assets::GetScene(m_ActiveScene);
			SerializeScene(filepath, activeScene);
		}
	}

	void EditorLayer::SaveActiveScene()
	{
		const Asset& activeSceneAsset = Assets::GetAsset(m_ActiveScene);
		Scene* activeScene = Assets::GetScene(m_ActiveScene);
		SerializeScene(activeSceneAsset.Path, activeScene);
	}

	void EditorLayer::CloseActiveScene()
	{
		SaveActiveScene();
		Assets::ReleaseAsset(m_ActiveScene);
		m_ActiveScene = 0;
		m_SceneHierarchyPanel.SetActiveScene(0);
		m_InspectorPanel.SetActiveScene(0);
	}
}