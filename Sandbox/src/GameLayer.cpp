#include "GameLayer.h"
#include <imgui/imgui.h>

GameLayer::GameLayer()
	: Layer("Game")
{
	auto& window = Vision::Application::Get().GetWindow();
	float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
	m_CameraController = Vision::CreateScope<Vision::OrthographicCameraController>(aspectRatio);
}

void GameLayer::OnAttach()
{
	using namespace Vision;

	m_SpriteShader = Shader::CreateFromFile("Assets/Shaders/Sprite.glsl");
	m_BatchedSpriteShader = Shader::CreateFromFile("Assets/Shaders/BatchedSprite.glsl");
	m_Brick = Texture::CreateFromFile("Assets/Textures/brick.png");
	m_Wall = Texture::CreateFromFile("Assets/Textures/wall.jpg");
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnEnable()
{
}

void GameLayer::OnDisable()
{
}

void GameLayer::OnUpdate(float dt)
{
	using namespace Vision;

	RenderCommand::Clear(ClearColorBuffer | ClearDepthBuffer);

	m_CameraController->OnUpdate(dt);

	Renderer2D::BeginBatchedScene(m_BatchedSpriteShader, m_CameraController->GetCamera());

	for (int y = 0; y < m_Rows; ++y)
	{
		for (int x = 0; x < m_Cols; ++x)
		{
			float spacing = (m_TileOffset + m_TileSize);
			glm::vec3 position(spacing * x, spacing * y, 0.0f);
			Renderer2D::SubmitQuad(position, 0.0f, glm::vec2(m_TileSize, m_TileSize), glm::vec4(1.0f, 0.0f, 0.0f, 1.0f));
		}
	}

	Renderer2D::EndBatchedScene();
}

void GameLayer::OnImGuiRender()
{
	using namespace Vision;

	m_CameraController->OnImGuiRender();

	ImGui::Begin("Graphics Settings");

	ImGui::ColorPicker4("Clear Color", &m_ClearColor.r);
	RenderCommand::SetClearColor(m_ClearColor);

	ImGui::ColorPicker4("Tint Color", &m_TintColor.r);

	ImGui::SliderFloat("Tile Offset", &m_TileOffset, 0.01f, 100.0f);
	ImGui::SliderFloat("Tile Size", &m_TileSize, 0.01f, 100.0f);
	
	ImGui::SliderInt("Tile Rows", &m_Rows, 1, 100);
	ImGui::SliderInt("Tile Cols", &m_Cols, 1, 100);

	ImGui::End();
}

void GameLayer::OnEvent(Vision::Event& e)
{
	m_CameraController->OnEvent(e);
}