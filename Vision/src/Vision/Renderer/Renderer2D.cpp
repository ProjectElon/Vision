#include "pch.h"
#include "Vision/Renderer/Renderer2D.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/matrix_decompose.hpp>

namespace Vision
{
	void Renderer2D::Init()
	{
		uint32 white = 0xffffffff;

		InitTexture(&s_QuadData.WhitePixel,
					  &white,
			          1,
			          1,
			          PixelFormat::RGBA);

		s_QuadData.MaxTextureSlots = Renderer::GetMaxTextureSlots();

		for (uint32 textureSlotIndex = 0;
			 textureSlotIndex < s_QuadData.MaxTextureSlots;
			 textureSlotIndex++)
		{
			s_QuadData.Samplers[textureSlotIndex] = textureSlotIndex;
			s_QuadData.TextureSlots[textureSlotIndex] = -1;
		}

		VertexLayout quadVertexlayout(
		{
			{ DataType::Float2, "a_Position",     false },
			{ DataType::Float4, "a_Color", 	      false },
			{ DataType::Float2, "a_TextureCoord", false },
			{ DataType::Float,  "a_TextureIndex", false },
			{ DataType::Float,  "a_EntityIndex",  false }
		});

		s_QuadData.VertexBase = new QuadVertex[4 * s_QuadData.MaxCount];
		s_QuadData.CurrentVertex = s_QuadData.VertexBase;

		CreateVertexBuffer(&s_QuadData.VertexBuffer,
						   nullptr,
						   4 * sizeof(QuadVertex) * s_QuadData.MaxCount,
						   BufferUsage::Dynamic);

		SetVertexBufferLayout(&s_QuadData.VertexBuffer,
							  &quadVertexlayout);

		uint32 maxQuadIndexCount = s_QuadData.MaxCount * 6;
		uint32* indices = new uint32[maxQuadIndexCount];

		for (uint32 quadIndex = 0, offset = 0;
			 quadIndex < maxQuadIndexCount;
			 quadIndex += 6)
		{
			indices[quadIndex + 0] = 0 + offset;
			indices[quadIndex + 1] = 1 + offset;
			indices[quadIndex + 2] = 2 + offset;

			indices[quadIndex + 3] = 2 + offset;
			indices[quadIndex + 4] = 3 + offset;
			indices[quadIndex + 5] = 0 + offset;

			offset += 4;
		}

		CreateIndexBuffer(&s_QuadData.IndexBuffer,
						  indices,
						  maxQuadIndexCount * sizeof(uint32));

		delete[] indices;
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_QuadData.VertexBase;
	}

	void Renderer2D::BeginScene(const glm::mat4& cameraTransform,
								const glm::mat4& cameraProjection,
								Shader* quadShader)
	{
		s_SceneData.CameraPosition = cameraTransform[3];
		s_SceneData.ViewProjection = cameraProjection * glm::inverse(cameraTransform);

		BindShader(quadShader);
		SetShaderUniformIntArray(quadShader,
								 "u_Textures",
								 s_QuadData.Samplers,
								 s_QuadData.MaxTextureSlots);

		SetShaderUniformMatrix4(quadShader,
								"u_ViewProj",
								glm::value_ptr(s_SceneData.ViewProjection));
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform,
						      const glm::vec4& color,
						      uint32 entityIndex)
	{
		DrawTexture(transform,
					&s_QuadData.WhitePixel,
					color,
					{ 0.0f, 0.0f },
					{ 1.0f, 1.0f },
					entityIndex);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position,
							  float32 rotationAngle,
							  const glm::vec2& scale,
							  const glm::vec4& color,
							  uint32 entityIndex)
	{
		glm::mat4 transform = CreateQuadTransform(position,
												  rotationAngle,
												  scale);
		DrawQuad(transform, color, entityIndex);
	}

	void Renderer2D::DrawTexture(const glm::vec3& position,
								 float32 rotationAngle,
								 const glm::vec2& scale,
								 const Texture* texture,
								 const glm::vec4& color,
								 const glm::vec2& bottomLeftUV,
								 const glm::vec2& topRightUV,
								 uint32 entityIndex)
	{
		glm::mat4 transform = CreateQuadTransform(position,
											      rotationAngle,
											      scale);

		DrawTexture(transform,
			        texture,
			        color,
					bottomLeftUV,
					topRightUV,
					entityIndex);
	}

	void Renderer2D::DrawTexture(const glm::mat4& transform,
								 const Texture* texture,
								 const glm::vec4& color,
								 const glm::vec2& bottomLeftUV,
								 const glm::vec2& topRightUV,
								 uint32 entityIndex)
	{
		if (s_QuadData.Count >= s_QuadData.MaxCount)
		{
			FlushQuadBatch();
		}

		float32 textureIndex = (float32)AssginTextureSlot(texture);

		glm::vec3 v0 = transform * glm::vec4(-0.5f, -0.5f, 1.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4( 0.5f, -0.5f, 1.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4( 0.5f,  0.5f, 1.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(-0.5f,  0.5f, 1.0f, 1.0f);

		glm::vec2 uv0 = bottomLeftUV;
		glm::vec2 uv1 = glm::vec2(topRightUV.x, bottomLeftUV.y);
		glm::vec2 uv2 = topRightUV;
		glm::vec2 uv3 = glm::vec2(bottomLeftUV.x, topRightUV.y);

		SubmitQuadVertex({ v0, color, uv0, textureIndex, (float32)entityIndex });
		SubmitQuadVertex({ v1, color, uv1, textureIndex, (float32)entityIndex });
		SubmitQuadVertex({ v2, color, uv2, textureIndex, (float32)entityIndex });
		SubmitQuadVertex({ v3, color, uv3, textureIndex, (float32)entityIndex });

		s_QuadData.Count++;
	}

	void Renderer2D::DrawString(BitmapFont* font,
	                            const std::string& text,
	                            glm::vec2 position,
	                            const glm::vec4& color,
	                            uint32 entityIndex)
	{
		for (size_t characterIndex = 0; characterIndex < text.length(); characterIndex++)
		{
			char character = text[characterIndex];

			if (character >= font->FirstCharacter && character <= font->LastCharacter)
			{
				stbtt_aligned_quad quad;

				stbtt_GetPackedQuad(font->Glyphs,
									font->Atlas.Width,
									font->Atlas.Height,
									character - font->FirstCharacter,
									&position.x,
									&position.y,
									&quad,
									1); // 1 for opengl, 0 for dx3d

				float xp = (quad.x1 + quad.x0) / 2.0f;
				float yp = (quad.y1 + quad.y0) / 2.0f;

				float sx = (quad.x1 - quad.x0);
				float sy = (quad.y1 - quad.y0);

				glm::vec2 uv0 = { quad.s0, quad.t0 };
				glm::vec2 uv1 = { quad.s1, quad.t1 };

				DrawTexture(glm::vec3(xp, yp, 0.0f),
							0.0f,
							glm::vec2(sx, sy),
							&font->Atlas,
							color,
							uv0,
							uv1,
							entityIndex);
			}
			else
			{
				VnCoreInfo("UnPacked character: {0} in font atlas bitmap", character);
			}
		}
	}

	void Renderer2D::EndScene()
	{
		FlushQuadBatch();
	}

	uint32 Renderer2D::AssginTextureSlot(const Texture* texture)
	{
		int32 textureIndex = -1;

		for (uint32 index = 0;
			 index < s_QuadData.CurrentTextureIndex;
			 index++)
		{
			if (texture->RendererID == s_QuadData.TextureSlots[index])
			{
				textureIndex = index;
				break;
			}
		}

		if (textureIndex == -1)
		{
			if (s_QuadData.CurrentTextureIndex >= s_QuadData.MaxTextureSlots)
			{
				FlushQuadBatch();

				s_QuadData.CurrentTextureIndex = 0;

				memset(s_QuadData.TextureSlots,
					   0,
				       sizeof(uint32) * s_QuadData.MaxTextureSlots);
			}

			textureIndex = s_QuadData.CurrentTextureIndex;

			s_QuadData.TextureSlots[textureIndex] = texture->RendererID;
			s_QuadData.CurrentTextureIndex++;
		}

		BindTexture(texture, textureIndex);

		return textureIndex;
	}

	glm::mat4 Renderer2D::CreateQuadTransform(const glm::vec3& position,
									          float32 rotationAngle,
									          const glm::vec2& scale)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position)   *
							  glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f)) *
							  glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		return transform;
	}

	void Renderer2D::SubmitQuadVertex(const QuadVertex& vertex)
	{
		s_QuadData.CurrentVertex->Position     = vertex.Position;
		s_QuadData.CurrentVertex->Color        = vertex.Color;
		s_QuadData.CurrentVertex->TextureCoord = vertex.TextureCoord;
		s_QuadData.CurrentVertex->TextureIndex = vertex.TextureIndex;
		s_QuadData.CurrentVertex->EntityIndex  = vertex.EntityIndex;
		s_QuadData.CurrentVertex++;
	}

	void Renderer2D::FlushQuadBatch()
	{
		if (s_QuadData.Count > 0)
		{
			BindVertexBuffer(&s_QuadData.VertexBuffer);
			BindIndexBuffer(&s_QuadData.IndexBuffer);

			uint32 dataSize = 4 * sizeof(QuadVertex) * s_QuadData.Count;

			SetVertexBufferData(&s_QuadData.VertexBuffer,
								(float*)s_QuadData.VertexBase,
								dataSize);

 			Renderer::DrawIndexed(&s_QuadData.VertexBuffer,
								  &s_QuadData.IndexBuffer,
 								  s_QuadData.Count * 6,
 								  Primitive::Triangles);

			s_QuadData.CurrentVertex = s_QuadData.VertexBase;
			s_QuadData.Count = 0;
		}
	}

	SceneData Renderer2D::s_SceneData; // @(Harlequin): this should be on the heap
	QuadData  Renderer2D::s_QuadData; // @(Harlequin): this should be on the heap
}