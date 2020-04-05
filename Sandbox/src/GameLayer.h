#pragma once

#include <Vision.h>

// temparary for testing 
struct Entity
{
	std::string Name;
	glm::vec3 Position = { 0.0f, 0.0f, 0.0f };
	float Rotation = 0.0f;
	glm::vec2 Scale = { 1.0f, 1.0f };
	Vision::Ref<Vision::Sprite> Sprite; // Should be by default a pink textures
	int DrawOrder = 0;
};

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

	glm::vec4 m_ClearColor = glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
	glm::vec4 m_WhiteColor = { 1.0f, 1.0f, 1.0f, 1.0f };
	
	std::vector<Entity*> m_Entities;
};