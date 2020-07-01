#include "pch.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Renderer/RenderCommand.h"
#include <glm/gtx/matrix_transform_2d.hpp>

namespace Vision
{
	void Renderer2D::Init()
	{
		InitQuadSetup();
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_QuadData.TextureSlots;
		delete[] s_QuadData.Samplers;
		delete[] s_QuadData.VertexBase;
	}

	void Renderer2D::BeginScene(const OrthographicCamera& camera, 
								const Ref<Shader>& quadShader)
	{
		s_SceneData.CameraPosition = camera.GetPosition();
		s_SceneData.ViewProjection = camera.GetViewProjection();

		quadShader->Bind();
		quadShader->SetIntArray("u_Textures", s_QuadData.Samplers, s_QuadData.MaxTextureSlots);
		quadShader->SetMatrix4("u_ViewProj", s_SceneData.ViewProjection);
		// quadShader->SetFloat3("u_CameraPosition", camera.GetPosition());
	}

	void Renderer2D::DrawQuad(const glm::vec2& position,
							  real32 rotationAngle,
							  const glm::vec2& scale,
							  const glm::vec4& color)					
	{
		DrawTexture(position,
					rotationAngle,
					scale,
					s_QuadData.WhitePixel,
					color);
	}

	void Renderer2D::DrawTexture(const glm::vec2& position,
								 real32 rotationAngle,
								 const glm::vec2& scale,
								 const Ref<Texture2D>& texture,
								 const glm::vec4& color)
	{
		if (s_QuadData.Count >= s_QuadData.MaxCount)
		{
			FlushQuadBatch();
		}

		real32 textureIndex = (real32)AssginTextureSlot(texture);
		
		glm::mat3 transform = CreateQuadTransform(position,
											      rotationAngle,
											      scale); 
		
		glm::vec2 v0 = transform * glm::vec3(-0.5f, -0.5f, 1.0f);
		glm::vec2 v1 = transform * glm::vec3( 0.5f, -0.5f, 1.0f);
		glm::vec2 v2 = transform * glm::vec3( 0.5f,  0.5f, 1.0f);
		glm::vec2 v3 = transform * glm::vec3(-0.5f,  0.5f, 1.0f);

		SubmitQuadVertex({ v0, color, glm::vec2(0.0f, 0.0f), textureIndex });
		SubmitQuadVertex({ v1, color, glm::vec2(1.0f, 0.0f), textureIndex });
		SubmitQuadVertex({ v2, color, glm::vec2(1.0f, 1.0f), textureIndex });
		SubmitQuadVertex({ v3, color, glm::vec2(0.0f, 1.0f), textureIndex });

		s_QuadData.Count++;
	}

	void Renderer2D::DrawSprite(const glm::vec2& position,
								real32 rotationAngle,
								const glm::vec2& scale,
								const Ref<Sprite>& sprite)
	{
		if (s_QuadData.Count >= s_QuadData.MaxCount)
		{
			FlushQuadBatch();
		}

		real32 textureIndex = (real32)AssginTextureSlot(sprite->Texture);

		glm::mat3 transform = CreateQuadTransform(position,
											      rotationAngle,
											      scale); 

		glm::vec2 v0 = transform * glm::vec3(-0.5f, -0.5f, 1.0f);
		glm::vec2 v1 = transform * glm::vec3( 0.5f, -0.5f, 1.0f);
		glm::vec2 v2 = transform * glm::vec3( 0.5f,  0.5f, 1.0f);
		glm::vec2 v3 = transform * glm::vec3(-0.5f,  0.5f, 1.0f);	

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

		s_QuadData.Count++;
 	}

	void Renderer2D::EndScene()
	{
		FlushQuadBatch();
	}

	void Renderer2D::InitQuadSetup()
	{
		uint32 white = 0xffffffff;

		s_QuadData.WhitePixel = Texture2D::Create(1, 1);
		s_QuadData.WhitePixel->SetData(&white, sizeof(uint32) * 1 * 1);	
		
		s_QuadData.MaxTextureSlots = Renderer::GetRendererAPI().GetMaxTextureSlots();		
		s_QuadData.Samplers = new int32[s_QuadData.MaxTextureSlots];
		s_QuadData.TextureSlots = new uint32[s_QuadData.MaxTextureSlots];
		
		for (uint32 i = 0; i < s_QuadData.MaxTextureSlots; i++)
		{
			s_QuadData.Samplers[i] = i;
			s_QuadData.TextureSlots[i] = -1;
		}

		VertexLayout layout(
		{
			{ RendererAPI::DataType::Float2, "a_Position",     false },
			{ RendererAPI::DataType::Float4, "a_Color", 	   false },
			{ RendererAPI::DataType::Float2, "a_TextureCoord", false },
			{ RendererAPI::DataType::Float,  "a_TextureIndex", false }
		});

		BufferProps vertexBufferDesc;
		vertexBufferDesc.Data = nullptr;
		vertexBufferDesc.DataType = RendererAPI::DataType::Float;
		vertexBufferDesc.Size = 4 * sizeof(QuadVertex) * s_QuadData.MaxCount;
		vertexBufferDesc.Usage = BufferUsage::Dynamic;

		s_QuadData.VertexBase = new QuadVertex[4 * s_QuadData.MaxCount];
		s_QuadData.CurrentVertex = s_QuadData.VertexBase;
		
		s_QuadData.VertexBuffer = VertexBuffer::Create(vertexBufferDesc);
		s_QuadData.VertexBuffer->SetLayout(layout);

		uint32 maxQuadIndexCount = s_QuadData.MaxCount * 6;
		uint32* indices = new uint32[maxQuadIndexCount];
		
		uint32 offset = 0;
		
		for (uint32 i = 0; i < maxQuadIndexCount; i += 6)
		{
			indices[i + 0] = 0 + offset;
			indices[i + 1] = 1 + offset;
			indices[i + 2] = 2 + offset;
			
			indices[i + 3] = 2 + offset;
			indices[i + 4] = 3 + offset;
			indices[i + 5] = 0 + offset;
			
			offset += 4;
		}

		BufferProps indexBufferDesc;
		indexBufferDesc.Data = indices;
		indexBufferDesc.DataType = RendererAPI::DataType::UInt;
		indexBufferDesc.Size = maxQuadIndexCount * sizeof(uint32);
		indexBufferDesc.Usage = BufferUsage::Static;
		
		s_QuadData.IndexBuffer = IndexBuffer::Create(indexBufferDesc);
		delete[] indices;
	}
	
	uint32 Renderer2D::AssginTextureSlot(const Ref<Texture2D>& texture)
	{
		int32 textureIndex = -1;

		for (uint32 i = 0; i < s_QuadData.CurrentTextureIndex; i++)
		{
			if (texture->GetRendererID() == s_QuadData.TextureSlots[i])
			{
				textureIndex = i;
				break;
			}
		}

		if (textureIndex == -1)
		{
			if (s_QuadData.CurrentTextureIndex >= s_QuadData.MaxTextureSlots)
			{
				FlushQuadBatch();

				s_QuadData.CurrentTextureIndex = 0;
				memset(s_QuadData.TextureSlots, 0, sizeof(uint32) * s_QuadData.MaxTextureSlots);
			}

			textureIndex = s_QuadData.CurrentTextureIndex;
			
			s_QuadData.TextureSlots[textureIndex] = texture->GetRendererID();
			s_QuadData.CurrentTextureIndex++;
		}

		texture->Bind(textureIndex);

		return textureIndex;
	}

	glm::mat3 Renderer2D::CreateQuadTransform(const glm::vec2& position,
									          real32 rotationAngle,
									          const glm::vec2& scale)
	{
		glm::mat3 transform =
			glm::translate(glm::mat3(1.0f), position)   *
			glm::rotate(glm::mat3(1.0f), rotationAngle) *
			glm::scale(glm::mat3(1.0f), scale);

		return transform;									
	}

	void Renderer2D::SubmitQuadVertex(const QuadVertex& vertex)
	{
		s_QuadData.CurrentVertex->Position     = vertex.Position;
		s_QuadData.CurrentVertex->Color        = vertex.Color;
		s_QuadData.CurrentVertex->TextureCoord = vertex.TextureCoord;
		s_QuadData.CurrentVertex->TextureIndex = vertex.TextureIndex;
		s_QuadData.CurrentVertex++;
	}
	
	void Renderer2D::FlushQuadBatch()
	{
		if (s_QuadData.Count > 0)
		{
			s_QuadData.VertexBuffer->Bind();
			s_QuadData.IndexBuffer->Bind();

			uint32 dataSize = 4 * sizeof(QuadVertex) * s_QuadData.Count;
			s_QuadData.VertexBuffer->SetData(s_QuadData.VertexBase, dataSize);
			
 			RenderCommand::DrawIndexed(s_QuadData.VertexBuffer, 
 									   s_QuadData.IndexBuffer,
 									   s_QuadData.Count * 6,
 									   RendererAPI::Primitive::Triangles);
			
			s_QuadData.CurrentVertex = s_QuadData.VertexBase;
			s_QuadData.Count = 0;
		}
	}
	
	SceneData Renderer2D::s_SceneData;
	QuadData  Renderer2D::s_QuadData;
}