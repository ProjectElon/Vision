#pragma once

#include <Vision.h>
#include <rapidjson/document.h>

#include "Widgets/Menubar.h"
#include "Widgets/SceneHierarchyPanel.h"
#include "Widgets/InspectorPanel.h"
#include "Widgets/ConsolePanel.h"
#include "Widgets/SceneViewPanel.h"
#include "Widgets/GameViewPanel.h"
#include "Widgets/Dialog.h"

namespace Vision
{
	class EditorLayer : public Layer
	{
    public:
		EditorLayer();
		~EditorLayer();

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(float deltaTime) override;
		void OnEvent(Event& event) override;
		void OnImGuiRender() override;

		bool OnKeyPressed(KeyPressedEvent& e);

		void OnFileChanged(FileWatcherAction action, std::string filepath, std::string oldpath);

		void NewScene(const std::string& filepath, uint32 maxEntityCount);
		void OpenScene();
		void SaveSceneAs(Scene* scene);
		void SaveScene(Scene* scene);
		void CloseScene(Scene* scene);

   	private:
		void LoadSettings();
		void SaveSettings();

   	private:
		Window* m_Window;

		Menubar             m_Menubar;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		InspectorPanel		m_InspectorPanel;
		ConsolePanel		m_ConsolePanel;
		SceneViewPanel		m_SceneViewPanel;
		GameViewPanel		m_GameViewPanel;
		Dialog				m_Dialog;

		rapidjson::Document    m_Settings;
		std::string			   m_LastScenePath;

		Scope<OrthographicCameraController> m_CameraController;

		Scope<FileWatcher> m_FileWatcher;

		TextureAtlasGrid m_PlayerAtlas;
		uint32 m_TextureIndex = 0;

		Asset* m_CheckboardTexture;
		Asset* m_PlayerTexture;
		Asset* m_SpriteShader;

		Ref<FrameBuffer> m_SceneFrameBuffer;
		Ref<FrameBuffer> m_GameFrameBuffer;
	};
}