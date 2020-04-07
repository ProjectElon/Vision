#include "pch.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Renderer/RenderCommand.h"

namespace Vision
{
	void Renderer2D::Init()
	{
		TextureProps ColorTextureProps;
		ColorTextureProps.Filter = FilterMode::Point;

		uint32_t white = 0xffffffff;
		s_Data.WhitePixel = Texture2D::Create(1, 1, ColorTextureProps);
		s_Data.WhitePixel->SetData(&white, sizeof(uint32_t) * 1 * 1);
		
		// TODO : to be abstracted (RenderCaps)
		s_Data.MaxTextureSlots = Renderer::GetRendererAPI().GetMaxTextureSlots();
		
		s_Data.Samplers = new int32_t[s_Data.MaxTextureSlots];
		
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			s_Data.Samplers[i] = i;
		}

		VertexLayout layout({
			{ API::DataType::Float3, "a_Position"     },
			{ API::DataType::Float4, "a_Color"        },
			{ API::DataType::Float2, "a_TextureCoord" },
			{ API::DataType::Float,  "a_TextureIndex" }
		});

		BufferProps VertexBufferDesc
		{
			nullptr,
			API::DataType::Float,
			sizeof(QuadVertex) * s_Data.MaxQuadVertexCount,
			BufferUsage::Dynamic
		};

		s_Data.QuadVertexBase = new QuadVertex[s_Data.MaxQuadVertexCount];
		s_Data.CurrentQuadVertex = s_Data.QuadVertexBase;
		
		s_Data.QuadVertexBuffer = VertexBuffer::Create(VertexBufferDesc);
		s_Data.QuadVertexBuffer->SetLayout(layout);

		uint32_t* indices = new uint32_t[s_Data.MaxQuadIndexCount];
		
		uint32_t offset = 0;

		for (uint32_t i = 0; i < s_Data.MaxQuadIndexCount; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;
			
			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;

			offset += 4;
		}

		BufferProps indexBufferDesc
		{
			indices,
			API::DataType::UInt,
			sizeof(uint32_t) * s_Data.MaxQuadIndexCount,
			BufferUsage::Static
		};

		s_Data.QuadIndexBuffer = IndexBuffer::Create(indexBufferDesc);
		delete[] indices;
	}

	void Renderer2D::Shutdown()
	{
		s_Data.QuadVertexBuffer.reset();
		s_Data.QuadIndexBuffer.reset();
		s_Data.WhitePixel.reset();

		delete[] s_Data.Samplers;
		delete[] s_Data.QuadVertexBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera, const Ref<Shader>& shader)
	{
		s_SceneData.CameraPosition = camera.GetPosition();
		s_SceneData.ViewProjection = camera.GetViewProjection();

		s_Data.WhitePixel->Bind(0);

		s_Data.QuadVertexBuffer->Bind();
		s_Data.QuadIndexBuffer->Bind();

		s_Data.QuadShader = shader;
		s_Data.QuadShader->Bind();
		s_Data.QuadShader->SetIntArray("u_Textures", s_Data.Samplers, s_Data.MaxTextureSlots);

		s_Data.QuadShader->SetMatrix4("u_ViewProj", s_SceneData.ViewProjection);
		// s_Data.QuadShader->SetFloat3("u_CameraPosition", camera.GetPosition());
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color)
	{
		if (s_Data.QuadCount >= s_Data.MaxQuadCount)
		{
			Flush();
		}

		float textureIndex = 0.0f; // 0 = White Texture

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		glm::vec3 v0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);

		SubmitQuadVertex({ v0, color, glm::vec2(0.0f, 0.0f), textureIndex });
		SubmitQuadVertex({ v1, color, glm::vec2(1.0f, 0.0f), textureIndex });
		SubmitQuadVertex({ v2, color, glm::vec2(1.0f, 1.0f), textureIndex });
		SubmitQuadVertex({ v3, color, glm::vec2(0.0f, 1.0f), textureIndex });

		s_Data.QuadCount++;
	}

	void Renderer2D::DrawTexture(const glm::vec3& position, float rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color)
	{
		if (s_Data.QuadCount >= s_Data.MaxQuadCount)
		{
			Flush();
		}

		float textureIndex = 0.0f;
		auto it = s_Data.TextureSlots.find(texture);

		if (it == s_Data.TextureSlots.end())
		{
			if (s_Data.CurrentTextureIndex >= s_Data.MaxTextureSlots)
			{
				Flush();
				
				s_Data.CurrentTextureIndex = 1;
				s_Data.TextureSlots.clear();
			}

			textureIndex = (float)s_Data.CurrentTextureIndex;
			s_Data.TextureSlots.emplace(texture, s_Data.CurrentTextureIndex);
			s_Data.CurrentTextureIndex++;
		}
		else
		{
			textureIndex = (float)it->second;
		}

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		glm::vec3 v0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

		SubmitQuadVertex({ v0, color, glm::vec2(0.0f, 0.0f), textureIndex });
		SubmitQuadVertex({ v1, color, glm::vec2(1.0f, 0.0f), textureIndex });
		SubmitQuadVertex({ v2, color, glm::vec2(1.0f, 1.0f), textureIndex });
		SubmitQuadVertex({ v3, color, glm::vec2(0.0f, 1.0f), textureIndex });

		s_Data.QuadCount++;
	}

	void Renderer2D::DrawSprite(const glm::vec3& position, float rotation, const glm::vec2& scale, const Ref<Sprite>& sprite)
	{
		if (s_Data.QuadCount >= s_Data.MaxQuadCount)
		{
			Flush();
		}

		float textureIndex = 0.0f;
		auto it = s_Data.TextureSlots.find(sprite->Texture);

		if (it == s_Data.TextureSlots.end())
		{
			if (s_Data.CurrentTextureIndex >= s_Data.MaxTextureSlots)
			{
				Flush();

				s_Data.CurrentTextureIndex = 1;
				s_Data.TextureSlots.clear();
			}

			textureIndex = (float)s_Data.CurrentTextureIndex;
			s_Data.TextureSlots.emplace(sprite->Texture, s_Data.CurrentTextureIndex);
			s_Data.CurrentTextureIndex++;
		}
		else
		{
			textureIndex = (float)it->second;
		}
		
		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		glm::vec3 v0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4(0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4(0.5f, 0.5f, 0.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(-0.5f, 0.5f, 0.0f, 1.0f);

		glm::vec2 uv0 = sprite->BottomLeftUV;
		glm::vec2 uv1 = glm::vec2(sprite->TopRightUV.x, sprite->BottomLeftUV.y);
		glm::vec2 uv2 = sprite->TopRightUV;
		glm::vec2 uv3 = glm::vec2(sprite->BottomLeftUV.x, sprite->TopRightUV.y);
		
		if (sprite->FlipX)
		{
			std::swap(uv0, uv1);
			std::swap(uv2, uv3);
		}

		if (sprite->FlipY)
		{
			std::swap(uv0, uv3);
			std::swap(uv1, uv2);
		}

		SubmitQuadVertex({ v0, sprite->Color, uv0, textureIndex });
		SubmitQuadVertex({ v1, sprite->Color, uv1, textureIndex });
		SubmitQuadVertex({ v2, sprite->Color, uv2, textureIndex });
		SubmitQuadVertex({ v3, sprite->Color, uv3, textureIndex });

		s_Data.QuadCount++;
 	}

	void Renderer2D::EndScene()
	{
		Flush();
	}

	void Renderer2D::Flush()
	{
		if (s_Data.QuadCount > 0)
		{
			for (auto& it : s_Data.TextureSlots)
			{
				it.first->Bind(it.second);
			}

			uint32_t dataSize = (uint8_t*)s_Data.CurrentQuadVertex - (uint8_t*)s_Data.QuadVertexBase;
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBase, dataSize);
			
 			RenderCommand::DrawIndexed(s_Data.QuadVertexBuffer, s_Data.QuadIndexBuffer, s_Data.QuadCount * 6);
			
			s_Data.CurrentQuadVertex = s_Data.QuadVertexBase;
			s_Data.QuadCount = 0;
		}
	}

	void Renderer2D::SubmitQuadVertex(const QuadVertex& quadVertex)
	{
		s_Data.CurrentQuadVertex->Position = quadVertex.Position;
		s_Data.CurrentQuadVertex->Color = quadVertex.Color;
		s_Data.CurrentQuadVertex->TextureCoord = quadVertex.TextureCoord;
		s_Data.CurrentQuadVertex->TextureIndex = quadVertex.TextureIndex;
		s_Data.CurrentQuadVertex++;
	}

	SceneData Renderer2D::s_SceneData;
	RendererData Renderer2D::s_Data;
}