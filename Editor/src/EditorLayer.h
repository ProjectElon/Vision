#pragma once

#include <Vision.h>

class EditorLayer : public Vision::Layer
{
public:
	EditorLayer();

	void OnAttach() override;
	void OnDetach() override;

	void OnUpdate(float deltaTime) override;
	void OnEvent(Vision::Event& e) override;
	void OnImGuiRender() override;

private:
	Vision::Scope<Vision::OrthographicCameraController> m_CameraController;

	Vision::Ref<Vision::Shader> m_SpriteShader;

	Vision::Ref<Vision::Texture2D> m_CheckboardTexture;
	Vision::Ref<Vision::Texture2D> m_CharacterTexture;

	Vision::Ref<Vision::Sprite>      m_CheckerboardSprite;
	Vision::Ref<Vision::SpriteAtlas> m_CharacterAtlas;

	Vision::Ref<Vision::Sprite> m_WalkAnimation[8];

	real32 m_WalkAnimationIndex = 0.0f;
	real32 m_WalkAnimationFrames = 12.0f;

	glm::vec4 m_WhiteColor;
	glm::vec4 m_ClearColor;

	glm::vec2 m_ViewportSize = { 800.0f, 600.0f };
	Vision::Ref<Vision::FrameBuffer> m_SceneFrameBuffer;

	bool m_IsViewportFocused = false;
	bool m_IsViewportHovered = false;
};