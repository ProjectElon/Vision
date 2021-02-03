#pragma once

#include <Vision.h>
#include "Widgets/Menubar.h"
#include "Widgets/SceneHierarchyPanel.h"
#include "Widgets/InspectorPanel.h"
#include "Widgets/SceneViewPanel.h"
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
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseWheelScrolled(MouseWheelScrolledEvent& e);

		void OnFileChanged(FileWatcherAction action,
						   std::string filepath,
						   std::string oldpath);

		void NewScene(const std::string& filepath, uint32 maxEntityCount);
		void OpenScene();
		void SaveActiveSceneAs();
		void SaveActiveScene();
		void CloseActiveScene();

   	private:
		friend class Menubar;
		friend class Dialog;

		Menubar             m_Menubar;
		SceneHierarchyPanel m_SceneHierarchyPanel;
		InspectorPanel		m_InspectorPanel;
		SceneViewPanel		m_SceneViewPanel;
		Dialog				m_Dialog;

		FrameBuffer m_SceneFrameBuffer;

		OrthographicCamera* m_SceneCamera;

		AssetID m_ActiveScene = 0;
		AssetID m_CheckboardTexture = 0;

		AssetID m_FiraCodeFont;
		AssetID m_SpriteShader = 0;
		AssetID m_FontShader = 0;

		void LoadSettings();
		void SaveSettings();
	};
}