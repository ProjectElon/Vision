#pragma once

#include <Vision.h>
#include <rapidjson/document.h>

#include "Menubar.h"
#include "SceneHierarchyPanel.h"
#include "InspectorPanel.h"
#include "ConsolePanel.h"
#include "SceneViewPanel.h"
#include "GameViewPanel.h"

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
		void OnEvent(Vision::Event& event) override;
		void OnImGuiRender() override;

	private:
		void LoadSettings();
		void SaveSettings();

	private:
		Window* m_Window;
		
		Menubar             m_Menubar;
		SceneHierarchyPanel m_SceneHierarchPanel;
		InspectorPanel		m_InspectorPanel;
		ConsolePanel		m_ConsolePanel;
		SceneViewPanel		m_SceneViewPanel;
		GameViewPanel		m_GameViewPanel;
		
		rapidjson::Document m_Settings;

		Vision::Scope<OrthographicCameraController> m_CameraController;

		Vision::Ref<Shader> m_SpriteShader;

		Vision::Ref<Texture2D> m_CheckboardTexture;
		Vision::Ref<Texture2D> m_PlayerTexture;

		SpriteComponent m_CheckerboardSprite;
		
		Ref<FrameBuffer> m_SceneFrameBuffer;
		Ref<FrameBuffer> m_GameFrameBuffer;

		std::unique_ptr<Scene> m_MainScene;
	};
}