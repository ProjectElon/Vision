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
		Renderer::API.SetVSync(true);
		//@Incomplete: Remove and add to allocators
		SceneFrameBuffer = VnAllocateStruct(FrameBuffer);
		SceneViewPanel.FrameBuffer = SceneFrameBuffer;
	}

	EditorLayer::~EditorLayer()
	{
	}

	void EditorLayer::OnAttach()
	{
		LoadVars();

		// WatchDirectory("Assets", VnBindWatcherFn(EditorLayer::OnFileChanged), true);
		WatchDirectory("", VnBindWatcherFn(EditorLayer::OnFileChanged), false);

		SceneViewPanel.EditorLayer = this;
	}

	void EditorLayer::OnDetach()
	{
		SaveVars();
	}

	void EditorLayer::OnFileChanged(DirectoryWatcherAction action,
									std::string filepath,
									std::string oldpath)
	{
		std::string extension = FileSystem::GetFileExtension(filepath, false);

		switch (action)
		{
			case DirectoryWatcherAction::FileAdded:
			{
			}
			break;

			case DirectoryWatcherAction::FileRemoved:
			{
			}
			break;

			case DirectoryWatcherAction::FileModified:
			{
				std::string cwd = FileSystem::GetCurrentWorkingDirectory();
				std::string fileRelativePath = filepath.substr(cwd.length() + 1);

				bool isFile = FileSystem::FileExists(fileRelativePath);

				if (isFile)
				{
					std::string extension = FileSystem::GetFileExtension(fileRelativePath, false);

					if (extension == "vars")
					{
						LoadVars();
					}
					else
					{
						Assets::ReloadAsset(fileRelativePath);
					}
				}
			}
			break;

			case DirectoryWatcherAction::FileRenamed:
			{
			}
			break;
		}
	}

	void EditorLayer::OnUpdate(float deltaTime)
	{
		bool isControlDown = Input::IsKeyDown(Key::LeftControl) ||
			Input::IsKeyDown(Key::RightControl);

		if (ActiveSceneID && !isControlDown)
		{
			SceneCamera->OnUpdate(deltaTime, SceneViewPanel.IsInteractable);
		}

		Renderer::API.BindFrameBuffer(SceneFrameBuffer);

		if (ActiveSceneID)
		{
			float32 values[] = { 0.1f, 0.1f, 0.1f, 1.0f };
			Renderer::API.ClearColorAttachment(SceneFrameBuffer, 0, values);

			int value = -1;
			Renderer::API.ClearColorAttachment(SceneFrameBuffer, 1, &value);

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
										static_cast<int32>(entity));
			});

			Renderer2D::EndScene();

			glm::mat4 ortho = glm::ortho(0.0f,
			                             static_cast<float32>(SceneFrameBuffer->Width),
									     static_cast<float32>(SceneFrameBuffer->Height),
									     0.0f);

			Renderer2D::BeginScene(glm::mat4(1.0f),
								   ortho,
								   Assets::GetShader(SpriteShaderID));


			Renderer2D::DrawString(Assets::GetBitmapFont(MainFontID),
								   "Vision Engine!",
								   glm::vec2(10.0f, 50.0f),
								   glm::vec4(0.9f, 0.3f, 0.0f, 1.0f));

			scene->EachGroup<TagComponent, TransformComponent>([this](Entity entity, auto& tag, auto& transform)
			{
				Renderer2D::DrawString(Assets::GetBitmapFont(DebugFontID),
								   	   std::string(tag.Tag.Data),
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
				    !ImGuizmo::IsOver() &&
					!ImGui::IsMouseDragging(ImGuiMouseButton_Left) &&
					!Input::IsKeyDown(Key::LeftAlt))
				{
					int32 pixel = Renderer::API.ReadPixel(SceneFrameBuffer,
					                                      1,
					                                      mouseX,
					                                      mouseY);

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
							editorState.SelectedEntityTag = tagComponent.Tag.Data;
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
			Renderer::API.ClearColorAttachment(SceneFrameBuffer, 0, values);
		}

		Renderer::API.UnbindFrameBuffer(SceneFrameBuffer);
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

		Menubar.OnImGuiRender();
		SceneHierarchyPanel.OnImGuiRender();
		InspectorPanel.OnImGuiRender();
		ContentBrowserPanel.OnImGuiRender();
		SceneViewPanel.OnImGuiRender();
		Dialog.OnImGuiRender();

		if (SceneViewPanel.IsViewportResized)
		{
			SceneCamera->SetViewportSize(SceneViewPanel.ViewportSize.x,
										 SceneViewPanel.ViewportSize.y);
		}
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
					OpenSceneDialog();
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
			SceneCamera->UpdateView();
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
		ContentBrowserPanel.ActiveSceneID = ActiveSceneID;
		InspectorPanel.ActiveSceneID = ActiveSceneID;
		SceneViewPanel.ActiveSceneID = ActiveSceneID;
	}

	void EditorLayer::OpenScene(const std::string& filepath)
	{
		if (ActiveSceneID)
		{
			SaveActiveScene();
			Assets::ReleaseAsset(ActiveSceneID);
		}

		ActiveSceneID = Assets::RequestAsset(filepath);
		SceneHierarchyPanel.ActiveSceneID = ActiveSceneID;
		ContentBrowserPanel.ActiveSceneID = ActiveSceneID;
		InspectorPanel.ActiveSceneID = ActiveSceneID;
		SceneViewPanel.ActiveSceneID = ActiveSceneID;
	}

	void EditorLayer::OpenSceneDialog()
	{
		std::string filepath = FileDialog::OpenFile("Vision Scene (*.vscene)", { "vscene" });

		if (!filepath.empty())
		{
			OpenScene(filepath);
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
		VnCoreInfo("Saving Active Scene: {0}", activeSceneAsset.Path);
	}

	void EditorLayer::CloseActiveScene()
	{
		SaveActiveScene();
		Assets::ReleaseAsset(ActiveSceneID);
		ActiveSceneID = 0;
		SceneHierarchyPanel.ActiveSceneID = 0;
		ContentBrowserPanel.ActiveSceneID = 0;
		InspectorPanel.ActiveSceneID = 0;
		SceneViewPanel.ActiveSceneID = 0;
	}

	void EditorLayer::LoadVars()
	{
		Vars& vars = Application::Get().Vars;
		vars.Load();

		Settings& settings = vars.Settings;

		if (SpriteShaderID)
		{
			Assets::ReleaseAsset(SpriteShaderID);
			SpriteShaderID = 0;
		}

		if (MainFontID)
		{
			Assets::ReleaseAsset(MainFontID);
			MainFontID = 0;
		}

		if (DebugFontID)
		{
			Assets::ReleaseAsset(DebugFontID);
			DebugFontID = 0;
		}


		SpriteShaderID = Assets::RequestAsset(std::string(settings.SpriteShaderPath.Data,
														  settings.SpriteShaderPath.Count));

		MainFontID = Assets::RequestAsset(std::string(settings.MainFontPath.Data,
													  settings.MainFontPath.Count));

		DebugFontID = Assets::RequestAsset(std::string(settings.DebugFontPath.Data,
													   settings.DebugFontPath.Count));

		std::string scenePath = vars.Settings.ScenePath.Data;

		if (scenePath != "none" && FileSystem::FileExists(scenePath))
		{
			ActiveSceneID = Assets::RequestAsset(scenePath);
			SceneHierarchyPanel.ActiveSceneID = ActiveSceneID;
			InspectorPanel.ActiveSceneID = ActiveSceneID;
			SceneViewPanel.ActiveSceneID = ActiveSceneID;
			ContentBrowserPanel.ActiveSceneID = ActiveSceneID;
		}

		Window& window = Application::Get().Window;

		const char* windowTitle = vars.Settings.WindowTitle.Data;
		SetWindowTitle(&window, windowTitle);
		SetWindowPosition(&window, vars.Settings.WindowX, vars.Settings.WindowY);
		SetWindowSize(&window, vars.Settings.WindowWidth, vars.Settings.WindowHeight);

		FrameBufferAttachmentSpecification SceneBufferSpecification;

		SceneBufferSpecification.Attachments =
		{
			FrameBufferTextureFormat_RGBA8,
			FrameBufferTextureFormat_RedInt32,
			FrameBufferTextureFormat_Depth24Stencil8
		};

		Renderer::API.InitFrameBuffer(SceneFrameBuffer,
									  SceneBufferSpecification,
									  vars.Settings.ViewportWidth,
									  vars.Settings.ViewportHeight);

		EditorState& editorState = Scene::EditorState;
		SceneCamera = &editorState.SceneCamera;

		float32 aspectRatio = static_cast<float32>(vars.Settings.ViewportWidth) /
							  static_cast<float32>(vars.Settings.ViewportHeight);

		SceneCamera->Init(aspectRatio, 45.0f);

		BitmapFont* firaCodeFont = Assets::GetBitmapFont(MainFontID);
		SetBitmapFontSize(firaCodeFont, vars.Settings.FontSize);

		BitmapFont* debugFont = Assets::GetBitmapFont(DebugFontID);
		SetBitmapFontSize(debugFont, vars.Settings.DebugFontSize);
	}

	void EditorLayer::SaveVars()
	{
		Vars& vars = Application::Get().Vars;

		std::string activeScenePath = "none";

		if (ActiveSceneID)
		{
			SaveActiveScene();

			const Asset& activeSceneAsset = Assets::GetAsset(ActiveSceneID);
			activeScenePath = activeSceneAsset.Path;
		}

		// vars.Settings.ScenePath = activeScenePath.c_str();
		AssignString(&vars.Settings.ScenePath, activeScenePath.c_str(), activeScenePath.length());

		vars.Settings.ViewportWidth = static_cast<uint32>(SceneViewPanel.ViewportSize.x);
		vars.Settings.ViewportHeight = static_cast<uint32>(SceneViewPanel.ViewportSize.y);

		vars.Save();
	}
}