#include "pch.h"
#include "Vision/Renderer/Renderer.h"

#include "Vision/Renderer/OpenGL/OpenGLRendererAPI.h"
#include "Vision/Renderer/RenderCommand.h"

#include <glm/gtc/matrix_transform.hpp>

namespace Vision
{
	Renderer::API Renderer::s_API = API::None;
	
	Scope<RendererAPI> Renderer::s_RendererAPI;

	SceneData Renderer::s_SceneData2D;
	Ref<VertexBuffer> Renderer::s_QuadVB;
	Ref<IndexBuffer> Renderer::s_QuadIB;
	Ref<Texture> Renderer::s_WhitePixel;

	void Renderer::Init()
	{
		switch (GetAPI())
		{
			case Renderer::API::OpenGL:
			{
				s_RendererAPI = MakeScope<OpenGLRendererAPI>();
			}
			break;
		}

		Vertex2D vertices[] =
		{
			{ glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 0.0f) },
			{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f) },
			{ glm::vec3(0.5f,  -0.5f, 0.0f), glm::vec2(1.0f, 1.0f) },
			{ glm::vec3(0.5f,   0.5f, 0.0f), glm::vec2(1.0f, 0.0f) }
		};

		VertexLayout layout({
			{ Shader::DataType::Float3, "Position", false },
			{ Shader::DataType::Float2, "TexCoord", false }
		});

		BufferProps VertexBufferDesc
		{
			vertices,
			Shader::DataType::Float,
			sizeof(vertices),
			BufferUsage::Static
		};

		s_QuadVB = VertexBuffer::Create(VertexBufferDesc);
		s_QuadVB->SetLayout(layout);

		uint32_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		BufferProps indexBufferDesc
		{
			indices,
			Shader::DataType::UInt,
			sizeof(indices),
			BufferUsage::Static
		};

		s_QuadIB = IndexBuffer::Create(indexBufferDesc);

		glm::vec4 colors[1] = 
		{
			glm::vec4(1.0f, 1.0f, 1.0f, 1.0f)
		};
		
		TextureProps ColorTextureProps;
		ColorTextureProps.Filter = FilterMode::Point;

		s_WhitePixel = Texture::Create(colors, 1, 1, ColorTextureProps);
	}

	void Renderer::Shutdown()
	{
		s_QuadVB.reset();
		s_QuadIB.reset();
		s_WhitePixel.reset();
		s_RendererAPI.release();
	}

	void Renderer::OnWindowResize(uint32_t width, uint32_t height)
	{
		s_RendererAPI->OnWindowResize(width, height);
	}

	void Renderer::BeginScene2D(const OrthographicCamera& camera)
	{
		s_SceneData2D.ViewProjection = camera.GetViewProjection();
		s_QuadVB->Bind();
		s_QuadIB->Bind();
	}
	
	void Renderer::DrawSprite(const Ref<Shader>& shader, 
							  const glm::vec3& position, 
							  float rotation, 
							  const glm::vec2& scale, 
							  const Ref<Texture>& sprite, 
							  const glm::vec4& tint)
	{
		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		shader->Bind();

		shader->UploadUniformMatrix4("u_Transform", transform);
		shader->UploadUniformMatrix4("u_ViewProj", s_SceneData2D.ViewProjection);
		
		sprite->Bind(0);
		shader->UploadUniformInt("u_MainTex", 0);
		shader->UploadUniformFloat4("u_TintColor", tint);

		s_RendererAPI->DrawIndexed(s_QuadVB, s_QuadIB);
	}

	void Renderer::DrawQuad(const Ref<Shader>& shader,
							const glm::vec3& position,
							float rotation,
							const glm::vec2& scale,
							const glm::vec4& color)
	{
		DrawSprite(shader, position, rotation, scale, s_WhitePixel, color);
	}
	
	void Renderer::EndScene2D()
	{
		s_QuadVB->UnBind();
		s_QuadIB->UnBind();
	}
}