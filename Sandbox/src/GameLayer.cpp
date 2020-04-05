#include "GameLayer.h"
#include "imgui.h"

GameLayer::GameLayer()
	: Layer("Game2D")
{
	auto& window = Vision::Application::Get().GetWindow();
	float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
	m_CameraController = Vision::CreateScope<Vision::OrthographicCameraController>(aspectRatio);
}

void GameLayer::OnAttach()
{
	m_SpriteShader = Vision::Shader::CreateFromFile("Assets/Shaders/Sprite.glsl");

	Vision::TextureProps props;
	props.WrapX = Vision::WrapMode::Repeat;
	props.WrapY = Vision::WrapMode::Repeat;
	props.Filter = Vision::FilterMode::Point;

	m_CheckerboardSprite = Vision::CreateRef<Vision::Sprite>("Checkerboard", 
		Vision::Texture2D::CreateFromFile("Assets/Textures/Checkerboard.png", props));
	
	m_CheckerboardSprite->TopRightUV = glm::vec2(100.0f, 100.0f);

	m_DirtSprite = Vision::CreateRef<Vision::Sprite>("Dirt Sprite", 
		Vision::Texture2D::CreateFromFile("Assets/Textures/dirt.png", props));

	m_WoodSprite = Vision::CreateRef<Vision::Sprite>("Wood Sprite",
		Vision::Texture2D::CreateFromFile("Assets/Textures/wood.png", props));

	Entity* checkerBoard = new Entity;
	checkerBoard->Name = "CheckerBoard";
	checkerBoard->Sprite = m_CheckerboardSprite;
	checkerBoard->Scale = glm::vec2(500.0f, 500.0f);
	checkerBoard->DrawOrder = -10;

	Entity* e0 = new Entity;
	e0->Name = "Dirt Tile";
	e0->Sprite = m_DirtSprite;
	e0->Position = { 1.0f, 0.0f, 0.0f };

	Entity* e1 = new Entity; 
	e1->Name = "Wood Tile";
	e1->Sprite = m_WoodSprite;
	e1->Position = { -1.0f, 0.0f, 0.0f };

	m_Entities.push_back(checkerBoard);
	m_Entities.push_back(e0);
	m_Entities.push_back(e1);
}

void GameLayer::OnDetach()
{
	for (auto e : m_Entities)
	{
		delete e;
	}
}

void GameLayer::OnUpdate(float dt)
{
	using namespace Vision;

	m_CameraController->OnUpdate(dt);

	RenderCommand::Clear(API::ClearColorBufferBit | API::ClearDepthBufferBit);

	Renderer2D::BeginScene(m_CameraController->GetCamera(), m_SpriteShader);

	std::sort(m_Entities.begin(), m_Entities.end(), [](Entity* e0, Entity* e1)
	{
		return e0->DrawOrder < e1->DrawOrder;
	});

	for (auto e : m_Entities)
	{
		Renderer2D::DrawSprite(e->Position, e->Rotation, e->Scale, e->Sprite);
	}

	Renderer2D::EndScene();
}

void GameLayer::OnImGuiRender()
{
	using namespace Vision;

	m_CameraController->OnImGuiRender();

	ImGui::Begin("Metrics");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	
	ImGui::Begin("Graphics Settings");
	ImGui::ColorPicker4("Clear Color", &m_ClearColor.r);
	RenderCommand::SetClearColor(m_ClearColor);
	ImGui::End();

	ImGui::Begin("Entities");
	
	/* 
		Note : you can't have two imgui widgets with the same label
		so we have to add some stuff per entity (ImGui::Text doesn't need that)
		can be solved by having an inspector window (editor)
	*/

	int index = 0;

	for (auto e : m_Entities)
	{
		std::ostringstream ss;
		ss << index;

		std::string cur = ss.str() + " ";

		std::string entityName = "Entity : " + e->Name + "\n";

		ImGui::Text(entityName.c_str());

		ImGui::InputFloat3((cur + "Position").c_str(), &e->Position.x);
		ImGui::InputFloat((cur + "Rotation").c_str(), &e->Rotation);
		ImGui::InputFloat2((cur + "Scale").c_str(), &e->Scale.x);

		ImGui::Text("\n");

		std::string spriteName = "Sprite : " + e->Sprite->GetName();
		ImGui::Text(spriteName.c_str());
		ImGui::ColorPicker4((cur + "Color").c_str(), &(e->Sprite->Color.r));
		ImGui::InputFloat2((cur + "Bottom Left Point").c_str(), &(e->Sprite->BottomLeftUV.x));
		ImGui::InputFloat2((cur + "Top Right Point").c_str(), &(e->Sprite->TopRightUV.x));
		ImGui::Checkbox((cur + "Flip X").c_str(), &(e->Sprite->FlipX));
		ImGui::Checkbox((cur + "Flip Y").c_str(), &(e->Sprite->FlipY));
		ImGui::InputInt((cur + "Draw Order").c_str(), &(e->DrawOrder));

		ImGui::Text("-----------------------------------------------\n\n");

		++index;
	}

	ImGui::End();
}

void GameLayer::OnEvent(Vision::Event& e)
{
	m_CameraController->OnEvent(e);
}