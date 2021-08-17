#pragma once

#include <Vision.h>

#include "Widgets/Menubar.h"
#include "Widgets/SceneHierarchyPanel.h"
#include "Widgets/InspectorPanel.h"
#include "Widgets/SceneViewPanel.h"
#include "Widgets/ContentBrowserPanel.h"
#include "Widgets/Dialog.h"

namespace Vision
{
	class EditorLayer : public Layer
	{
    public:
		Menubar               Menubar;
		SceneHierarchyPanel   SceneHierarchyPanel;
		ContentBrowserPanel   ContentBrowserPanel;
		InspectorPanel		  InspectorPanel;
		SceneViewPanel		  SceneViewPanel;
		Dialog				  Dialog;

		FrameBuffer* 	   SceneFrameBuffer;
		PerspectiveCamera* SceneCamera;

		AssetID ActiveSceneID  = 0;
		AssetID MainFontID     = 0;
		AssetID DebugFontID    = 0;
		AssetID SpriteShaderID = 0;

		EditorLayer();
		~EditorLayer();

		void OnAttach() override;
		void OnDetach() override;

		void OnUpdate(float32 deltaTime) override;
		void OnEvent(Event& e) override;
		void OnImGuiRender() override;

		bool OnKeyPressed(KeyPressedEvent& e);
		bool OnMouseWheelScrolled(MouseWheelScrolledEvent& e);

		void OnFileChanged(DirectoryWatcherAction action,
						   std::string filepath,
						   std::string oldpath);

		void NewScene(const std::string& filepath, uint32 maxEntityCount);
		void OpenScene(const std::string& filepath);
		void OpenSceneDialog();
		void SaveActiveSceneAs();
		void SaveActiveScene();
		void CloseActiveScene();

		void LoadVars();
		void SaveVars();
	};
}