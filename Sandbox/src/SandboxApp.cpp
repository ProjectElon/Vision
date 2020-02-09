#include "SandboxApp.h"
#include <imgui/imgui.h>

GameLayer::GameLayer()
	: Layer("Game")
{
}

void GameLayer::OnAttach()
{
	using namespace Vision;

	float vertices[] =
	{
		 0.0f,  0.5f, 0.0f,
		-0.5f, -0.5f, 0.0f,
		 0.5f, -0.5f, 0.0f
	};

	VertexLayout layout({ 
		{ Shader::DataType::Float3, "Position", false } 
	});

	BufferProps vbDesc
	{
		vertices,
		Shader::DataType::Float,
		sizeof(vertices),
		BufferUsage::Static
	};

	m_VB.reset(VertexBuffer::Create(vbDesc));
	m_VB->SetLayout(layout);

	uint32_t indices[] =
	{
		0, 1, 2
	};

	BufferProps ibDesc
	{
		indices,
		Shader::DataType::UInt,
		sizeof(indices),
		BufferUsage::Static
	};

	m_IB.reset(IndexBuffer::Create(ibDesc));

	std::string vertexSource = R"(
		#version 330 core
		
		layout (location = 0) in vec3 in_Position;
		
		void main()
		{
			gl_Position = vec4(in_Position, 1.0f);
		}
	)";

	std::string pixelSource = R"(
		#version 330 core

		out vec4 out_Color;
		
		void main()
		{
			out_Color = vec4(1.0f, 0.5f, 0.2f, 1.0f);
		}
	)";

	m_TriangleShader.reset(Shader::Create("Tringle", vertexSource, pixelSource));
	m_TriangleShader->Bind();
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
	RenderCommand::DrawIndexed(*(m_VB.get()), *(m_IB.get()));
}

void GameLayer::OnImGuiRender()
{
}

void GameLayer::OnEvent(Vision::Event& event)
{
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