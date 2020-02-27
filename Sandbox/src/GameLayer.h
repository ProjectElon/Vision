#pragma once

#include <Vision.h>

class GameLayer : public Vision::Layer
{
public:
	GameLayer();

	virtual void OnAttach() override;
	virtual void OnDetach() override;

	virtual void OnEnable() override;
	virtual void OnDisable() override;

	virtual void OnUpdate(float dt) override;
	virtual void OnImGuiRender() override;
	virtual void OnEvent(Vision::Event& e) override;

private:
	Vision::Ref<Vision::Shader> m_SpriteShader;
	Vision::Ref<Vision::Shader> m_BatchedSpriteShader;

	Vision::Ref<Vision::Texture> m_Brick;
	Vision::Ref<Vision::Texture> m_Wall;

	Vision::Scope<Vision::OrthographicCameraController> m_CameraController;

	glm::vec4 m_TintColor = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec4 m_ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	
	float m_TileOffset = 1.5f;
	float m_TileSize = 1.5f;

	int m_Rows = 10;
	int m_Cols = 10;
};