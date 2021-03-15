#include "EditorLayer.h"

#include <fstream>
#include <sstream>
#include <imgui.h>
#include <glm/gtc/matrix_transform.hpp>

#include <algorithm>
#include <ImGuizmo.h>

namespace Vision
{
	EditorLayer::EditorLayer()
		: Layer("Editor")
		, Menubar(this)
		, Dialog(this)
	{
		Renderer::SetVSync(true);
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		using namespace Vision;

		Vars& vars = Application::Get().Vars;
		vars.Load();

		std::string scenePath = vars.Settings.ScenePath.Text;

		if (scenePath != "none" && FileSystem::FileExists(scenePath))
		{
			ActiveSceneID = Assets::RequestAsset(scenePath);
			SceneHierarchyPanel.ActiveSceneID = ActiveSceneID;
			InspectorPanel.ActiveSceneID = ActiveSceneID;
			SceneViewPanel.ActiveSceneID = ActiveSceneID;
		}

		Window& window = Application::Get().Window;

		window.SetTitle(vars.Settings.WindowTitle.Text);
		window.SetPosition(vars.Settings.WindowX, vars.Settings.WindowY);
		window.SetSize(vars.Settings.WindowWidth, vars.Settings.WindowHeight);

		if (vars.Settings.IsWindowMaximized)
		{
			window.Maximize();
		}

		FrameBufferAttachmentSpecification SceneBufferSpecification;

		SceneBufferSpecification.Attachments =
		{
			FrameBufferTextureFormat::RGBA8,
			FrameBufferTextureFormat::RedInt32,
			FrameBufferTextureFormat::Depth24Stencil8
		};

		SceneFrameBuffer.Init(SceneBufferSpecification,
							  vars.Settings.ViewportWidth,
							  vars.Settings.ViewportHeight);

		SceneViewPanel.FrameBuffer = &SceneFrameBuffer;

		EditorState& editorState = Scene::EditorState;
		SceneCamera = &editorState.SceneCamera;

		float32 aspectRatio = static_cast<float32>(vars.Settings.ViewportWidth) /
							  static_cast<float32>(vars.Settings.ViewportHeight);

		SceneCamera->Init(aspectRatio, 45.0f);

		SpriteShaderID = Assets::RequestAsset("Assets/Shaders/Sprite.glsl");
		FontShaderID = Assets::RequestAsset("Assets/Shaders/Font.glsl");

		FiraCodeFontID = Assets::RequestAsset("Assets/Fonts/FiraCode-Regular.ttf");
		DebugFontID = Assets::RequestAsset("Assets/Fonts/TitilliumWeb-Regular.ttf");

		BitmapFont* firaCodeFont = Assets::GetBitmapFont(FiraCodeFontID);
		firaCodeFont->SetSize(vars.Settings.FontSize);

		BitmapFont* debugFont = Assets::GetBitmapFont(DebugFontID);
		debugFont->SetSize(vars.Settings.DebugFontSize);

		WatchDirectory("Assets", VnBindWatcherFn(EditorLayer::OnFileChanged));
	}

	void EditorLayer::OnDetach()
	{
		Vars& vars = Application::Get().Vars;

		std::string activeScenePath = "none";

		if (ActiveSceneID)
		{
			SaveActiveScene();

			const Asset& activeSceneAsset = Assets::GetAsset(ActiveSceneID);
			activeScenePath = activeSceneAsset.Path;
		}

		vars.Settings.ScenePath = activeScenePath.c_str();
		vars.Settings.ViewportWidth = static_cast<uint32>(SceneViewPanel.ViewportSize.x);
		vars.Settings.ViewportHeight = static_cast<uint32>(SceneViewPanel.ViewportSize.y);

		vars.Save();
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

		if (SceneViewPanel.IsInteractable)
		{
			bool isControlDown = Input::IsKeyDown(Key::LeftControl) ||
								 Input::IsKeyDown(Key::RightControl);

			if (ActiveSceneID && !isControlDown)
			{
				SceneCamera->OnUpdate(deltaTime);
			}
		}

		SceneFrameBuffer.Bind();

		if (ActiveSceneID)
		{
			float32 values[] = { 0.1f, 0.1f, 0.1f, 1.0f };
			SceneFrameBuffer.ClearColorAttachment(0, values);

			int value = -1;
			SceneFrameBuffer.ClearColorAttachment(1, &value);

			Renderer2D::BeginScene(SceneCamera->View,
								   SceneCamera->Projection,
								   Assets::GetShader(SpriteShaderID));

			Scene* scene = Assets::GetScene(ActiveSceneID);

			scene->EachGroup<TransformComponent, SpriteRendererComponent>([](Entity entity, auto& transform, auto& sprite)
			{
				Renderer2D::DrawTexture(transform.Position,
										transform.Rotation,
										transform.Scale,
										Assets::GetTexture(sprite.Texture),
										sprite.Color,
										sprite.BottomLeftUV,
										sprite.TopRightUV,
										(int32)entity);
			});

			Renderer2D::EndScene();

			glm::mat4 ortho = glm::ortho(0.0f,
			                             static_cast<float32>(SceneFrameBuffer.Width),
									     static_cast<float32>(SceneFrameBuffer.Height),
									     0.0f);

			Renderer2D::BeginScene(glm::mat4(1.0f),
								   ortho,
								   Assets::GetShader(FontShaderID));

			Renderer2D::DrawString(Assets::GetBitmapFont(FiraCodeFontID),
								   "Vision Engine!",
								   glm::vec2(10.0f, 50.0f),
								   glm::vec4(0.9f, 0.8f, 0.2f, 1.0f),
								   0);

			scene->EachGroup<TagComponent, TransformComponent>([this](Entity entity, auto& tag, auto& transform)
			{
				Renderer2D::DrawString(Assets::GetBitmapFont(DebugFontID),
								   	   std::string(tag.Tag),
									   transform.Position,
			                           glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
									   entity);
			});

			Renderer2D::EndScene();

			const glm::vec2& minBound = SceneViewPanel.ViewportBounds[0];
			const glm::vec2& maxBound = SceneViewPanel.ViewportBounds[1];

			glm::vec2 viewportSize = maxBound - minBound;

			auto [mouseXf, mouseYf] = ImGui::GetMousePos();

			mouseXf -= minBound.x;
			mouseYf -= minBound.y;

			mouseYf = viewportSize.y - mouseYf;

			auto mouseX = static_cast<int32>(mouseXf);
			auto mouseY = static_cast<int32>(mouseYf);

			EditorState& editorState = Scene::EditorState;

			if (mouseX >= 0 && mouseX < static_cast<int32>(viewportSize.x) &&
			    mouseY >= 0 && mouseY < static_cast<int32>(viewportSize.y))
			{
				if (Input::IsMouseButtonDown(Mouse::ButtonLeft) &&
				    !ImGuizmo::IsUsing() &&
				    !ImGuizmo::IsOver())
				{
					int32 pixel = SceneFrameBuffer.ReadPixel(1, mouseX, mouseY);

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

							if (tagIt != scene->Tags.end())
							{
								Entity seletedEntity = tagIt->second;

								if (seletedEntity != pixel)
								{
									shouldSelectEntity = true;
								}
							}
						}

						if (shouldSelectEntity)
						{
							auto& tagComponent = scene->GetComponent<TagComponent>(pixel);
							editorState.SelectedEntityTag = tagComponent.Tag;
						}
					}
					else
					{
						editorState.SelectedEntityTag = "";
					}
				}
			}
		}
		else
		{
			float32 values[] = { 0.0f, 0.0f, 0.0f, 0.0f };
			SceneFrameBuffer.ClearColorAttachment(0, values);
		}

		SceneFrameBuffer.Unbind();
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

		if (SceneViewPanel.IsViewportResized)
		{
			SceneCamera->SetViewportSize(SceneViewPanel.ViewportSize.x,
										 SceneViewPanel.ViewportSize.y);
		}

		Menubar.OnImGuiRender();
		SceneHierarchyPanel.OnImGuiRender();
		InspectorPanel.OnImGuiRender();
		SceneViewPanel.OnImGuiRender();
		Dialog.OnImGuiRender();
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
					if (ActiveSceneID)
					{
						SaveActiveSceneAs();
					}
				}
				else if (isControlDown)
				{
					if (ActiveSceneID)
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

			case Key::Delete:
			{
				if (ActiveSceneID && SceneHierarchyPanel.IsInteractable)
				{
					Scene* scene = Assets::GetScene(ActiveSceneID);

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
				Application& application = Application::Get();
				application.Close();
			}
			break;

			case Key::Q:
			{
				if (SceneViewPanel.IsInteractable && !ImGuizmo::IsUsing())
				{
					SceneViewPanel.GizmoType = -1; // cancel gizmos
				}
			}
			break;

			case Key::E:
			{
				if (SceneViewPanel.IsInteractable && !ImGuizmo::IsUsing())
				{
					SceneViewPanel.GizmoType = ImGuizmo::OPERATION::TRANSLATE;
				}
			}
			break;

			case Key::R:
			{
				if (SceneViewPanel.IsInteractable && !ImGuizmo::IsUsing())
				{
					SceneViewPanel.GizmoType = ImGuizmo::OPERATION::ROTATE;
				}
			}
			break;

			case Key::T:
			{
				if (SceneViewPanel.IsInteractable && !ImGuizmo::IsUsing())
				{
					SceneViewPanel.GizmoType = ImGuizmo::OPERATION::SCALE;
				}
			}
			break;
		}

		return false;
	}

	bool EditorLayer::OnMouseWheelScrolled(MouseWheelScrolledEvent& e)
	{
		if (SceneViewPanel.IsInteractable)
		{
			float32 mouseWheelDelta = -e.GetYOffset() * 0.1f;
			SceneCamera->Zoom(mouseWheelDelta);
		}

		return false;
	}

	void EditorLayer::NewScene(const std::string& filepath, uint32 maxEntityCount)
	{
		if (ActiveSceneID)
		{
			SaveActiveScene();
			Assets::ReleaseAsset(ActiveSceneID);
		}

		SceneCamera->Reset();

		Scene* tempScene = new Scene;
		CreateScene(tempScene, maxEntityCount);
		SerializeScene(filepath, tempScene);

		DestroyScene(tempScene);
		delete tempScene;

		ActiveSceneID = Assets::RequestAsset(filepath);
		SceneHierarchyPanel.ActiveSceneID = ActiveSceneID;
		InspectorPanel.ActiveSceneID = ActiveSceneID;
		SceneViewPanel.ActiveSceneID = ActiveSceneID;
	}

	void EditorLayer::OpenScene()
	{
		std::string filepath = FileDialog::OpenFile("Vision Scene (*.vscene)", { "vscene" });

		if (!filepath.empty())
		{
			if (ActiveSceneID)
			{
				SaveActiveScene();
				Assets::ReleaseAsset(ActiveSceneID);
			}

			ActiveSceneID = Assets::RequestAsset(filepath);
			SceneHierarchyPanel.ActiveSceneID = ActiveSceneID;
			InspectorPanel.ActiveSceneID = ActiveSceneID;
			SceneViewPanel.ActiveSceneID = ActiveSceneID;
		}
	}

	void EditorLayer::SaveActiveSceneAs()
	{
		std::string filepath = FileDialog::SaveFile("Vision Scene (*.vscene)", "vscene");

		if (!filepath.empty())
		{
			Scene* activeScene = Assets::GetScene(ActiveSceneID);
			SerializeScene(filepath, activeScene);
		}
	}

	void EditorLayer::SaveActiveScene()
	{
		const Asset& activeSceneAsset = Assets::GetAsset(ActiveSceneID);
		Scene* activeScene = Assets::GetScene(ActiveSceneID);
		SerializeScene(activeSceneAsset.Path, activeScene);
	}

	void EditorLayer::CloseActiveScene()
	{
		SaveActiveScene();
		Assets::ReleaseAsset(ActiveSceneID);
		ActiveSceneID = 0;
		SceneHierarchyPanel.ActiveSceneID = 0;
		InspectorPanel.ActiveSceneID = 0;
		SceneViewPanel.ActiveSceneID = 0;
	}
}