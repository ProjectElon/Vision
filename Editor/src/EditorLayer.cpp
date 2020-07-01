#include "EditorLayer.h"
#include "imgui.h"

EditorLayer::EditorLayer()
	: Layer("Editor")
	, m_ClearColor(1.0f, 0.0f, 1.0f, 1.0f)
	, m_WhiteColor(1.0f, 1.0f, 1.0f, 1.0f)
{
	using namespace Vision;
	
	auto& window = Application::Get().GetWindow();
	window.SetVSync(true);
	window.SetTitle("Editor");
}

void EditorLayer::OnAttach()
{
	using namespace Vision;

	float aspectRatio = (float)props.Width / (float)props.Height;
	m_CameraController = CreateScope<OrthographicCameraController>(aspectRatio, 1.0f);

	m_SpriteShader = Shader::CreateFromFile("Assets/Shaders/Sprite.glsl");
		
	TextureProps tiled;

	tiled.WrapX  = WrapMode::Repeat;
	tiled.WrapY  = WrapMode::Repeat;
	tiled.Filter = FilterMode::Point;
	
	TextureProps transparent;
	transparent.WrapX  = WrapMode::ClampToEdge;
	transparent.WrapY  = WrapMode::ClampToEdge;
	transparent.Filter = FilterMode::Bilinear;

	std::string texturePath = "Assets/Textures/";

	m_CheckboardTexture = Texture2D::CreateFromFile(texturePath + "Checkerboard.png", tiled);
	m_CharacterTexture  = Texture2D::CreateFromFile(texturePath + "character_femaleAdventurer_sheetHD.png", transparent);

	m_CheckerboardSprite = CreateRef<Sprite>("Checkerboard", m_CheckboardTexture);
	m_CheckerboardSprite->TopRightUV = glm::vec2(50.0f, 50.0f);

	m_CharacterAtlas = CreateRef<SpriteAtlas>("Character", m_CharacterTexture);

	m_CharacterAtlas->SetSprite("Walk0", 0,    1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk1", 192,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk2", 384,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk3", 576,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk4", 768,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk5", 960,  1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk6", 1152, 1024, 192, 256);
	m_CharacterAtlas->SetSprite("Walk7", 1344, 1024, 192, 256);

	m_WalkAnimation[0] = m_CharacterAtlas->GetSprite("Walk0");
	m_WalkAnimation[1] = m_CharacterAtlas->GetSprite("Walk1");
	m_WalkAnimation[2] = m_CharacterAtlas->GetSprite("Walk2");
	m_WalkAnimation[3] = m_CharacterAtlas->GetSprite("Walk3");
	m_WalkAnimation[4] = m_CharacterAtlas->GetSprite("Walk4");
	m_WalkAnimation[5] = m_CharacterAtlas->GetSprite("Walk5");
	m_WalkAnimation[6] = m_CharacterAtlas->GetSprite("Walk6");
	m_WalkAnimation[7] = m_CharacterAtlas->GetSprite("Walk7");


	FrameBufferProps props = {};
	props.Width = 800;
	props.Height = 600;
	m_SceneFrameBuffer = FrameBuffer::Create(props);
}

void EditorLayer::OnDetach()
{
}

void EditorLayer::OnUpdate(float deltaTime)
{
	using namespace Vision;

	if (deltaTime >= 0.5f)
	{
		deltaTime = 0.5f;
	}

	m_CameraController->OnUpdate(deltaTime);

	static bool RWasDown = false;
	
	if (Input::IsKeyDown(VN_KEY_R))
	{
		RWasDown = true;
	}

	if (Input::IsKeyUp(VN_KEY_R) && RWasDown)
	{
		m_SpriteShader->Reload();
		RWasDown = false;
	}

	m_SceneFrameBuffer->Bind();

	RenderCommand::Clear(RendererAPI::ColorBuffer);

	Renderer2D::BeginScene(m_CameraController->GetCamera(), m_SpriteShader);	
	
	Renderer2D::DrawSprite(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec2(100.0f, 100.0f), m_CheckerboardSprite);
	
	m_WalkAnimationIndex += deltaTime * m_WalkAnimationFrames;
	
	while (m_WalkAnimationIndex >= 7.0f)
	{
		m_WalkAnimationIndex -= 7.0f;
	}
	
	uint32_t animationIndex = (uint32_t)m_WalkAnimationIndex;
	Renderer2D::DrawSprite(glm::vec3(0.0f, 0.0f, 0.0f), 0.0f, glm::vec2(1.0f, 1.0f), m_WalkAnimation[animationIndex]);

	Renderer2D::EndScene();

	m_SceneFrameBuffer->UnBind();
}

void EditorLayer::OnEvent(Vision::Event& e)
{
	m_CameraController->OnEvent(e);
}

void EditorLayer::OnImGuiRender()
{
	using namespace Vision;
	
	ImGui::Begin("Metrics");
	ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	ImGui::End();
	
	ImGui::Begin("Graphics Settings");
	ImGui::ColorEdit4("Clear Color", &m_ClearColor.r);
	RenderCommand::SetClearColor(m_ClearColor);
	ImGui::End();

	ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
	ImGui::Begin("Viewport");
	ImGui::PopStyleVar();

	ImVec2 currentViewportSize = ImGui::GetContentRegionAvail();

	uint32 texture = m_SceneFrameBuffer->GetColorAttachmentRendererId();
	ImGui::Image((void*)(intptr_t)texture, { m_ViewportSize.x, m_ViewportSize.y }, ImVec2(0, 1), ImVec2(1, 0));
	
	if (m_ViewportSize != glm::vec2(currentViewportSize.x, currentViewportSize.y))
	{
		uint32 width = (uint32)currentViewportSize.x;
		uint32 height = (uint32)currentViewportSize.y;
		m_CameraController->Resize(width, height);
		m_SceneFrameBuffer->Resize(width, height);
		m_ViewportSize = { currentViewportSize.x, currentViewportSize.y };
	}
	
	ImGui::End();
}