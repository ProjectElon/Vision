#include "SandboxApp.h"

GameLayer::GameLayer()
	: Layer("Game")
{
	auto& window = Vision::Application::Get().GetWindow();
	float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
	m_CameraController = MakeScope<Vision::OrthographicCameraController>(aspectRatio);
}

void GameLayer::OnAttach()
{
	using namespace Vision;

	m_SpriteShader = Shader::CreateFromFile("Assets/Shaders/Sprite.shader");
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

	Renderer::BeginScene2D(m_CameraController->GetCamera());
	
	Renderer::DrawSprite(m_SpriteShader, { 0.0f, 0.0f, 0.0f }, 0.0f, { 1.0f, 1.0f }, m_Brick, m_TintColor);
	Renderer::DrawSprite(m_SpriteShader, { 5.0f, 5.0f, 0.0f }, 0.0f, { 5.0f, 5.0f }, m_Wall, m_TintColor);
	Renderer::DrawQuad(m_SpriteShader,   {-5.0f, 0.0f, 0.0f }, 45.0f, { 2.0f, 2.0f }, glm::vec4(1.0f, 0.0f, 1.0f, 1.0f));

	Renderer::EndScene2D();
}

void GameLayer::OnImGuiRender()
{
	using namespace Vision;

	m_CameraController->OnImGuiRender();

	ImGui::Begin("Graphics Settings");
	
	ImGui::ColorPicker4("Clear Color", &m_ClearColor.r);
	RenderCommand::SetClearColor(m_ClearColor);

	ImGui::ColorPicker4("Tint Color", &m_TintColor.r);
	
	ImGui::End();
}

void GameLayer::OnEvent(Vision::Event& e)
{
	m_CameraController->OnEvent(e);
}

Sandbox::Sandbox()
{
	PushLayer(new GameLayer);
}

Sandbox::~Sandbox()
{
}

Vision::Application* Vision::CreateApplication()
{
	return new Sandbox();
}