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
	
	Vision::Ref<Vision::Sprite> m_CheckerboardSprite;
	
	Vision::Ref<Vision::Sprite> m_DirtSprite;
	Vision::Ref<Vision::Sprite> m_WoodSprite;

	Vision::Ref<Vision::SpriteAtlas> m_CharacterAtlas;
	Vision::Ref<Vision::Sprite> m_CharacterAnimation[8];
	float m_AnimationIndex = 0.0f;
	float m_AnimationFrames = 12.0f;

	glm::vec4 m_ClearColor;
	glm::vec4 m_WhiteColor;
};