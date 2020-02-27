#include "pch.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Renderer/RenderCommand.h"

namespace Vision
{
	Renderer2D::SceneData Renderer2D::s_SceneData;	
	Renderer2D::RendererData Renderer2D::s_RendererData;
	const uint32_t Renderer2D::s_MaxBatchedQuadCount;

	void Renderer2D::Init()
	{
		TextureProps ColorTextureProps;
		ColorTextureProps.Filter = FilterMode::Point;

		uint32_t white = 0xffffffff;
		s_RendererData.WhitePixel = Texture::Create(1, 1, ColorTextureProps);
		s_RendererData.WhitePixel->SetData(&white, sizeof(uint32_t) * 1 * 1);

		InitQuadResources();
		InitBatchedQuadResources();
	}

	void Renderer2D::InitQuadResources()
	{
		RendererData& data = s_RendererData;

		QuadVertex vertices[] =
		{
			{ glm::vec3(-0.5f,  0.5f, 0.0f), glm::vec2(0.0f, 0.0f) },
			{ glm::vec3(-0.5f, -0.5f, 0.0f), glm::vec2(0.0f, 1.0f) },
			{ glm::vec3(0.5f,  -0.5f, 0.0f), glm::vec2(1.0f, 1.0f) },
			{ glm::vec3(0.5f,   0.5f, 0.0f), glm::vec2(1.0f, 0.0f) }
		};

		VertexLayout layout({
			{ Shader::DataType::Float3, "Position" },
			{ Shader::DataType::Float2, "TexCoord" }
		});

		BufferProps VertexBufferDesc
		{
			vertices,
			Shader::DataType::Float,
			sizeof(vertices),
			BufferUsage::Static
		};

		data.QuadVB = VertexBuffer::Create(VertexBufferDesc);
		data.QuadVB->SetLayout(layout);

		uint16_t indices[] =
		{
			0, 1, 2,
			0, 2, 3
		};

		BufferProps indexBufferDesc
		{
			indices,
			Shader::DataType::UShort,
			sizeof(indices),
			BufferUsage::Static
		};

		data.QuadIB = IndexBuffer::Create(indexBufferDesc);
	}

	void Renderer2D::InitBatchedQuadResources()
	{
		RendererData& data = s_RendererData;

		static BatchedQuadVertex vertices[s_MaxBatchedQuadCount];
		data.QuadVertexPtr = vertices;
		data.CurrentQuadVertexPtr = vertices;

		VertexLayout layout({
			{ Shader::DataType::Float3, "Position" },
			{ Shader::DataType::Float4, "Color"    },
			{ Shader::DataType::Float2, "TexCoord" },
			{ Shader::DataType::Float,  "TexID"    }
		});

		BufferProps VertexBufferDesc
		{
			nullptr,
			Shader::DataType::Float,
			sizeof(vertices),
			BufferUsage::Dynamic
		};

		data.BatchedQuadVB = VertexBuffer::Create(VertexBufferDesc);
		data.BatchedQuadVB->SetLayout(layout);

		static uint32_t indices[s_MaxBatchedQuadCount * 6];
		uint32_t offset = 0;

		for (size_t i = 0; i < s_MaxBatchedQuadCount * 6; i += 6, offset += 4)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;
			indices[i + 3] = 0 + offset;
			indices[i + 4] = 2 + offset;
			indices[i + 5] = 3 + offset;
		}

		BufferProps indexBufferDesc
		{
			indices,
			Shader::DataType::UInt,
			sizeof(indices),
			BufferUsage::Static
		};

		data.BatchedQuadIB = IndexBuffer::Create(indexBufferDesc);
	}

	void Renderer2D::Shutdown()
	{
		s_RendererData.QuadVB.reset();
		s_RendererData.QuadIB.reset();
		s_RendererData.WhitePixel.reset();
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera)
	{
		s_SceneData.CameraPosition = camera.GetPosition();
		s_SceneData.ViewProjection = camera.GetViewProjection();
		
		RendererData& data = s_RendererData;

		data.QuadVB->Bind();
		data.QuadIB->Bind();
	}

	void Renderer2D::EndScene()
	{
		RendererData& data = s_RendererData;

		data.QuadVB->UnBind();
		data.QuadIB->UnBind();
	}

	void Renderer2D::DrawQuad(const Ref<Shader>& shader, const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture>& texture, const glm::vec4& tint)
	{
		RendererData& data = s_RendererData;

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

		shader->Bind();

		shader->SetMatrix4("u_Transform", transform);
		shader->SetMatrix4("u_ViewProj", s_SceneData.ViewProjection);

		texture->Bind(0);
		shader->SetInt("u_MainTex", 0);
		shader->SetFloat4("u_TintColor", tint);

		RenderCommand::DrawIndexed(data.QuadVB, data.QuadIB, data.QuadIB->GetIndexCount());
	}

	void Renderer2D::DrawQuad(const Ref<Shader>& shader, const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		DrawQuad(shader, position, rotation, size, s_RendererData.WhitePixel, color);
	}

	void Renderer2D::BeginBatchedScene(const Ref<Shader>& shader, const OrthographicCamera& camera)
	{
		RendererData& data = s_RendererData;

		s_SceneData.CameraPosition = camera.GetPosition();
		s_SceneData.ViewProjection = camera.GetViewProjection();

		data.BatchedSpriteShader = shader;

		data.BatchedQuadVB->Bind();
		data.BatchedQuadIB->Bind();
		data.WhitePixel->Bind(0);

		data.BatchedSpriteShader->Bind();
		data.BatchedSpriteShader->SetMatrix4("u_ViewProj", camera.GetViewProjection());
	}

	void Renderer2D::SubmitQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture>& texture, const glm::vec4& tint)
	{
		RendererData& data = s_RendererData;

		if (data.CurrentBatchedQuadCount >= s_MaxBatchedQuadCount)
		{
			FlushBatch();
		}

		auto it = data.TextureSlots.find(texture->GetName());
		uint32_t texID = -1;

		if (it == data.TextureSlots.end())
		{
			if (data.CurrentTextureSlot >= s_MaxTextureSlots)
			{
				FlushBatch();
			}

			texture->Bind(data.CurrentTextureSlot);
			data.TextureSlots.emplace(texture->GetName(), data.CurrentTextureSlot);
			texID = data.CurrentTextureSlot;
			data.CurrentTextureSlot++;
		}
		else
		{
			texID = it->second;
		}

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

		glm::vec3 v0 = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);

		SubmitQuadVertex(v0, glm::vec2(0.0f, 0.0f), texID, tint);
		SubmitQuadVertex(v1, glm::vec2(0.0f, 1.0f), texID, tint);
		SubmitQuadVertex(v2, glm::vec2(1.0f, 1.0f), texID, tint);
		SubmitQuadVertex(v3, glm::vec2(1.0f, 0.0f), texID, tint);

		data.CurrentBatchedQuadCount++;
	}

	void Renderer2D::SubmitQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color)
	{
		RendererData& data = s_RendererData;

		if (data.CurrentBatchedQuadCount >= s_MaxBatchedQuadCount)
		{
			FlushBatch();
		}

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), rotation, glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(size.x, size.y, 1.0f));

		glm::vec3 v0 = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);

		SubmitQuadVertex(v0, glm::vec2(0.0f, 0.0f), 0.0f, color);
		SubmitQuadVertex(v1, glm::vec2(1.0f, 1.0f), 0.0f, color);
		SubmitQuadVertex(v2, glm::vec2(1.0f, 0.0f), 0.0f, color);
		SubmitQuadVertex(v3, glm::vec2(1.0f, 0.0f), 0.0f, color);

		data.CurrentBatchedQuadCount++;
	}

	void Renderer2D::EndBatchedScene()
	{
		FlushBatch();
	}

	void Renderer2D::FlushBatch()
	{
		RendererData& data = s_RendererData;
		
		uint32_t sizeInBytes = sizeof(BatchedQuadVertex) * data.CurrentBatchedQuadCount * 4;
		data.BatchedQuadVB->SetSubData(data.QuadVertexPtr, sizeInBytes);
		
		for (int i = 0; i < data.CurrentTextureSlot; ++i)
		{
			std::stringstream ss;
			ss << "u_Textures[" << i << "]";
			data.BatchedSpriteShader->SetInt(ss.str(), i);
		}
		
		RenderCommand::DrawIndexed(data.BatchedQuadVB, data.BatchedQuadIB, data.CurrentBatchedQuadCount * 6);

		data.CurrentQuadVertexPtr = data.QuadVertexPtr;
		data.CurrentBatchedQuadCount = 0;
		data.CurrentTextureSlot = 1;
		data.TextureSlots.clear();
	}

	void Renderer2D::SubmitQuadVertex(const glm::vec3& position, const glm::vec2& texCoord, float texID, const glm::vec4& color)
	{
		RendererData& data = s_RendererData;

		data.CurrentQuadVertexPtr->Position = position;
		data.CurrentQuadVertexPtr->Color = color;
		data.CurrentQuadVertexPtr->TexCoord = texCoord;
		data.CurrentQuadVertexPtr->TexID = texID;

		data.CurrentQuadVertexPtr++;
	}
}