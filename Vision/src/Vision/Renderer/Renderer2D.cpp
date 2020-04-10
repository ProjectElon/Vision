#include "pch.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Renderer/RenderCommand.h"

namespace Vision
{
	void Renderer2D::Init()
	{
		uint32_t white = 0xffffffff;
		s_Data.WhitePixel = Texture2D::Create(1, 1);
		s_Data.WhitePixel->SetData(&white, sizeof(uint32_t) * 1 * 1);
		
		// TODO : to be abstracted (RenderCaps)
		// s_Data.MaxTextureSlots = Renderer::GetRendererAPI().GetMaxTextureSlots();

		// NOTE : when i set s_Data.s_Data.MaxTextureSlots >= 2 the bug with the textures stops

		s_Data.MaxTextureSlots = 1;

		s_Data.Samplers = new int32_t[s_Data.MaxTextureSlots];
		s_Data.TextureSlots = new uint32_t[s_Data.MaxTextureSlots];
		
		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			s_Data.Samplers[i] = i;
			s_Data.TextureSlots[i] = 0;
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
		delete[] s_Data.TextureSlots;
		delete[] s_Data.Samplers;
		delete[] s_Data.QuadVertexBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera, const Ref<Shader>& shader)
	{
		s_SceneData.CameraPosition = camera.GetPosition();
		s_SceneData.ViewProjection = camera.GetViewProjection();

		s_Data.QuadVertexBuffer->Bind();
		s_Data.QuadIndexBuffer->Bind();

		shader->Bind();
		shader->SetIntArray("u_Textures", s_Data.Samplers, s_Data.MaxTextureSlots);
		shader->SetMatrix4("u_ViewProj", s_SceneData.ViewProjection);
		// shader->SetFloat3("u_CameraPosition", camera.GetPosition());
	}

	void Renderer2D::DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color)
	{
		if (s_Data.QuadCount >= s_Data.MaxQuadCount)
		{
			Flush();
		}

		float textureIndex = (float)AssginTextureSlot(s_Data.WhitePixel);

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

		float textureIndex = (float)AssginTextureSlot(texture);

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

	void Renderer2D::DrawSprite(const glm::vec3& position, float rotation, const glm::vec2& scale, const Ref<Sprite>& sprite)
	{
		if (s_Data.QuadCount >= s_Data.MaxQuadCount)
		{
			Flush();
		}

		float textureIndex = (float)AssginTextureSlot(sprite->Texture);

		glm::mat4 transform =
			glm::translate(glm::mat4(1.0f), position) *
			glm::rotate(glm::mat4(1.0f), glm::radians(rotation), glm::vec3(0.0f, 0.0f, 1.0f)) *
			glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		glm::vec3 v0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);

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
			uint32_t dataSize = 4 * sizeof(QuadVertex) * s_Data.QuadCount;
			s_Data.QuadVertexBuffer->SetData(s_Data.QuadVertexBase, dataSize);
			
 			RenderCommand::DrawIndexed(s_Data.QuadVertexBuffer, s_Data.QuadIndexBuffer, s_Data.QuadCount * 6);
			
			s_Data.CurrentQuadVertex = s_Data.QuadVertexBase;
			s_Data.QuadCount = 0;
		}
	}

	uint32_t Renderer2D::AssginTextureSlot(const Ref<Texture2D>& texture)
	{
		int32_t textureIndex = -1;

		for (uint32_t i = 0; i < s_Data.MaxTextureSlots; i++)
		{
			if (texture->GetRendererID() == s_Data.TextureSlots[i])
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == -1)
		{
			if (s_Data.CurrentTextureIndex >= s_Data.MaxTextureSlots)
			{
				Flush();

				s_Data.CurrentTextureIndex = 0;
				memset(s_Data.TextureSlots, 0, sizeof(uint32_t) * s_Data.MaxTextureSlots);
			}

			s_Data.TextureSlots[s_Data.CurrentTextureIndex] = texture->GetRendererID();
			texture->Bind(s_Data.CurrentTextureIndex);
			textureIndex = s_Data.CurrentTextureIndex;
			s_Data.CurrentTextureIndex++;
		}

		return textureIndex;
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