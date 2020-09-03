#pragma once

#include <Vision.h>
#include <rapidjson/document.h>

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
		void OnEvent(Vision::Event& e) override;
		void OnImGuiRender() override;

	private:
		Window* m_Window;

		Vision::Scope<OrthographicCameraController> m_CameraController;

		Vision::Ref<Shader> m_SpriteShader;

		Vision::Ref<Texture2D> m_CheckboardTexture;
		Vision::Ref<Sprite>    m_CheckerboardSprite;
		
		glm::vec4 m_WhiteColor;

		glm::vec4 m_ClearColor = {};
		glm::vec4 m_LastClearColor = {};

		glm::vec2 m_ViewportSize = { 800.0f, 600.0f };
		Ref<FrameBuffer> m_SceneFrameBuffer;

		bool m_IsViewportFocused = false;
		bool m_IsViewportHovered = false;
		bool m_LastIsVSyncEnabled = false;
		bool m_IsVSyncEnabled = false;

		rapidjson::Document m_Settings;
	};
}