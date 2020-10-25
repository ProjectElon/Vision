#pragma once

#include <Vision.h>
#include <rapidjson/document.h>

#include "Widgets/Menubar.h"
#include "Widgets/SceneHierarchyPanel.h"
#include "Widgets/InspectorPanel.h"
#include "Widgets/ConsolePanel.h"
#include "Widgets/SceneViewPanel.h"
#include "Widgets/GameViewPanel.h"

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

		rapidjson::Document    m_Settings;
		
		Vision::Scope<OrthographicCameraController> m_CameraController;

		Vision::Ref<Shader> m_SpriteShader;

		Vision::Ref<Texture2D> m_CheckboardTexture;
		Vision::Ref<Texture2D> m_PlayerTexture;

		SpriteRendererComponent m_CheckerboardSprite;

		Ref<FrameBuffer> m_SceneFrameBuffer;
		Ref<FrameBuffer> m_GameFrameBuffer;
	};
}