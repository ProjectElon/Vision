#include "GameLayer.h"
#include "imgui.h"

GameLayer::GameLayer()
	: Layer("Game2D")
	, m_ClearColor(1.0f, 0.0f, 1.0f, 1.0f)
	, m_WhiteColor(1.0f, 1.0f, 1.0f, 1.0f)
{
	auto& window = Vision::Application::Get().GetWindow();
	float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
	m_CameraController = Vision::CreateScope<Vision::OrthographicCameraController>(aspectRatio);
}

void GameLayer::OnAttach()
{
	m_SpriteShader = Vision::Shader::CreateFromFile("Assets/Shaders/Sprite.glsl");

	Vision::TextureProps tiled;
	tiled.WrapX = Vision::WrapMode::Repeat;
	tiled.WrapY = Vision::WrapMode::Repeat;
	tiled.Filter = Vision::FilterMode::Point;

	Vision::TextureProps transparent;
	transparent.WrapX = Vision::WrapMode::ClampToEdge;
	transparent.WrapY = Vision::WrapMode::ClampToEdge;
	transparent.Filter = Vision::FilterMode::Point;

	m_CheckerboardSprite = Vision::CreateRef<Vision::Sprite>("Checkerboard", 
		Vision::Texture2D::CreateFromFile("Assets/Textures/Checkerboard.png", tiled));

	m_CheckerboardSprite->TopRightUV = glm::vec2(50.0f, 50.0f);

	/*
	m_DirtSprite = Vision::CreateRef<Vision::Sprite>("Dirt Sprite", 
		Vision::Texture2D::CreateFromFile("Assets/Textures/dirt.png", props));

	m_WoodSprite = Vision::CreateRef<Vision::Sprite>("Wood Sprite",
		Vision::Texture2D::CreateFromFile("Assets/Textures/wood.png", props));
	*/

	m_CharacterAtlas = Vision::CreateRef<Vision::SpriteAtlas>("Character",
		Vision::Texture2D::CreateFromFile("Assets/Textures/character_femaleAdventurer_sheetHD.png", transparent));

	m_CharacterAtlas->SetSprite("Walk0", 0,    1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk1", 192,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk2", 384,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk3", 576,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk4", 768,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk5", 960,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk6", 1152, 1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk7", 1344, 1024, 192, 256);

	m_CharacterAnimation[0] = m_CharacterAtlas->GetSprite("Walk0");
	m_CharacterAnimation[1] = m_CharacterAtlas->GetSprite("Walk1");
	m_CharacterAnimation[2] = m_CharacterAtlas->GetSprite("Walk2");
	m_CharacterAnimation[3] = m_CharacterAtlas->GetSprite("Walk3");
	m_CharacterAnimation[4] = m_CharacterAtlas->GetSprite("Walk4");
	m_CharacterAnimation[5] = m_CharacterAtlas->GetSprite("Walk5");
	m_CharacterAnimation[6] = m_CharacterAtlas->GetSprite("Walk6");
	m_CharacterAnimation[7] = m_CharacterAtlas->GetSprite("Walk7");
}

void GameLayer::OnDetach()
{
}

void GameLayer::OnUpdate(float dt)
{
	using namespace Vision;

	m_CameraController->OnUpdate(dt);

	RenderCommand::Clear(API::ClearColorBufferBit | API::ClearDepthBufferBit);

	Renderer2D::BeginScene(m_CameraController->GetCamera(), m_SpriteShader);
	
	Renderer2D::DrawSprite(glm::vec3(0.0f, 0.0f, -0.1f), 0.0f, glm::vec2(100.0f, 100.0f), m_CheckerboardSprite);
	
	m_AnimationIndex += dt * m_AnimationFrames;
	
	if (m_AnimationIndex > 7.0f)
	{
		m_AnimationIndex -= 7.0f;
	}
		
	uint32_t animIndex = (uint32_t)m_AnimationIndex;
	Renderer2D::DrawSprite(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec2(1.0f, 1.0f), m_CharacterAnimation[animIndex]);
		
	Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
	/*
		Note : you can't have two imgui widgets with the same label
		so we have to add some stuff per entity (ImGui::Text doesn't need that)
		can be solved by having an inspector window (editor)
	*/
	using namespace Vision;

	ImGui::Begin("Metrics");
	
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
	ImGui::End();
	
	ImGui::Begin("Graphics Settings");
	
	ImGui::ColorEdit4("Clear Color", &m_ClearColor.r);
	RenderCommand::SetClearColor(m_ClearColor);

	ImGui::End();
}

void GameLayer::OnEvent(Vision::Event& e)
{
	m_CameraController->OnEvent(e);
}