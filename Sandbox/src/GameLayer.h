#pragma once

#include <Vision.h>

class GameLayer : public Vision::Layer
{
public:
	GameLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Vision::Event& e) override;

private:
	Vision::Scope<Vision::OrthographicCameraController> m_CameraController;

	Vision::Ref<Vision::Shader> m_SpriteShader;

	Vision::Ref<Vision::Texture2D> m_CheckboardTexture;
	Vision::Ref<Vision::Texture2D> m_CharacterTexture;

	Vision::Ref<Vision::Sprite> m_CheckerboardSprite;
	Vision::Ref<Vision::SpriteAtlas> m_CharacterAtlas;

	Vision::Ref<Vision::Sprite> m_WalkAnimation[8];
	Vision::Ref<Vision::Sprite> m_RunAnimation[3];
	Vision::Ref<Vision::Sprite> m_AttackAnimation[3];

	float m_WalkAnimationIndex = 0.0f;
	float m_WalkAnimationFrames = 12.0f;
	
	float m_RunAnimationIndex = 0.0f;
	float m_RunAnimationFrames = 10.0f;

	float m_AttackAnimationIndex = 0.0f;
	float m_AttackAnimationFrames = 10.0f;
	
	glm::vec4 m_ClearColor;
	glm::vec4 m_WhiteColor;
};