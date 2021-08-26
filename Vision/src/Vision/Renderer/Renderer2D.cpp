#include "pch.hpp"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Renderer/Font.h"
#include "Vision/Entity/Scene.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Vision
{
	void Renderer2D::Initialize()
	{
		uint32 white = 0xffffffff;

		Renderer::API.InitTexture(&QuadData.WhitePixel,
								  &white,
								  1,
								  1,
								  PixelFormat_RGBA,
								  WrapMode_Repeat,
								  WrapMode_Repeat,
								  FilterMode_Bilinear);

		QuadData.MaxTextureSlots = 32;

		for (uint32 textureSlotIndex = 0;
			 textureSlotIndex < QuadData.MaxTextureSlots;
			 textureSlotIndex++)
		{
			QuadData.Samplers[textureSlotIndex] = textureSlotIndex;
			QuadData.TextureSlots[textureSlotIndex] = -1;
		}

		VertexLayout quadVertexlayout =
		{
			{ ShaderDataType_Float3, "a_Position",     false },
			{ ShaderDataType_Float4, "a_Color", 	   false },
			{ ShaderDataType_Float2, "a_TextureCoord", false },
			{ ShaderDataType_UInt32, "a_TextureIndex", false },
			{ ShaderDataType_Int32,  "a_EntityIndex",  false },
		};

		QuadData.VertexBase = new QuadVertex[4 * QuadData.MaxCount];
		QuadData.CurrentVertex = QuadData.VertexBase;

		Renderer::API.InitVertexBuffer(&QuadData.VertexBuffer,
									   nullptr,
									   4 * sizeof(QuadVertex) * QuadData.MaxCount,
									   BufferUsage::Dynamic);

		Renderer::API.SetVertexBufferLayout(&QuadData.VertexBuffer, &quadVertexlayout);

		uint32 maxQuadIndexCount = QuadData.MaxCount * 6;
		uint16* indices = new uint16[maxQuadIndexCount];

		VnCoreAssert(maxQuadIndexCount <= std::numeric_limits<uint16>::max(),
		             "can't use index buffer with uint16 values");

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

		Renderer::API.InitIndexBuffer16(&QuadData.IndexBuffer,
										indices,
										maxQuadIndexCount * sizeof(uint16),
										BufferUsage::Static);

		delete[] indices;
	}

	void Renderer2D::Shutdown()
	{
		delete[] QuadData.VertexBase;
	}

	void Renderer2D::BeginScene(const glm::mat4& cameraView,
								const glm::mat4& cameraProjection,
								Shader* shader)
	{
		SceneData.ViewProjection = cameraProjection * cameraView;

		Renderer::API.BindShader(shader);
		Renderer::API.SetUniformIntArray(shader,
										 "u_Textures",
										 QuadData.Samplers,
										 QuadData.MaxTextureSlots);

		Renderer::API.SetUniformMatrix4(shader,
										"u_ViewProj",
										glm::value_ptr(SceneData.ViewProjection));
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform,
						      const glm::vec4& color,
						      int32 entityIndex)
	{
		DrawTexture(transform,
					&QuadData.WhitePixel,
					color,
					{ 0.0f, 0.0f },
					{ 1.0f, 1.0f },
					entityIndex);
	}

	void Renderer2D::DrawQuad(const glm::vec3& position,
							  float32 rotationAngle,
							  const glm::vec2& scale,
							  const glm::vec4& color,
							  int32 entityIndex)
	{
		glm::mat4 transform = CreateQuadTransform(position,
												  rotationAngle,
												  scale);
		DrawQuad(transform, color, entityIndex);
	}

	void Renderer2D::DrawTexture(const glm::vec3& position,
								 float32 rotationAngle,
								 const glm::vec2& scale,
								 Texture* texture,
								 const glm::vec4& color,
								 const glm::vec2& bottomLeftUV,
								 const glm::vec2& topRightUV,
								 int32 entityIndex)
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

	void Renderer2D::DrawTexture(const glm::vec3& position,
								 const glm::vec3& rotation,
								 const glm::vec3& scale,
								 Texture* texture,
								 const glm::vec4& color,
								 const glm::vec2& bottomLeftUV,
								 const glm::vec2& topRightUV,
								 int32 entityIndex)
	{
		glm::mat4 transform = CreateQuadTransform(position,
												  rotation,
												  scale);

		DrawTexture(transform,
					texture,
					color,
					bottomLeftUV,
					topRightUV,
					entityIndex);
	}

	void Renderer2D::DrawTexture(const glm::mat4& transform,
								 Texture* texture,
								 const glm::vec4& color,
								 const glm::vec2& bottomLeftUV,
								 const glm::vec2& topRightUV,
								 int32 entityIndex)
	{
		if (QuadData.Count >= QuadData.MaxCount)
		{
			FlushQuadBatch();
		}

		uint32 textureIndex = AssginTextureSlot(texture);

		glm::vec3 v0 = transform * glm::vec4(-0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v1 = transform * glm::vec4( 0.5f, -0.5f, 0.0f, 1.0f);
		glm::vec3 v2 = transform * glm::vec4( 0.5f,  0.5f, 0.0f, 1.0f);
		glm::vec3 v3 = transform * glm::vec4(-0.5f,  0.5f, 0.0f, 1.0f);

		glm::vec2 uv0 = bottomLeftUV;
		glm::vec2 uv1 = glm::vec2(topRightUV.x, bottomLeftUV.y);
		glm::vec2 uv2 = topRightUV;
		glm::vec2 uv3 = glm::vec2(bottomLeftUV.x, topRightUV.y);

		SubmitQuadVertex({ v0, color, uv0, textureIndex, entityIndex });
		SubmitQuadVertex({ v1, color, uv1, textureIndex, entityIndex });
		SubmitQuadVertex({ v2, color, uv2, textureIndex, entityIndex });
		SubmitQuadVertex({ v3, color, uv3, textureIndex, entityIndex });

		QuadData.Count++;
	}

	void Renderer2D::DrawString(BitmapFont* font,
	                            const std::string& text,
	                            glm::vec2 position,
	                            const glm::vec4& color,
	                            int32 entityIndex)
	{
		for (memorysize characterIndex = 0;
		     characterIndex < text.length();
		     characterIndex++)
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

				float32 xp = (quad.x1 + quad.x0) / 2.0f;
				float32 yp = (quad.y1 + quad.y0) / 2.0f;

				float32 sx = (quad.x1 - quad.x0);
				float32 sy = (quad.y1 - quad.y0);

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
				VnCoreInfo("unpacked character: {0} in font atlas bitmap", character);
			}
		}
	}

	void Renderer2D::EndScene()
	{
		FlushQuadBatch();
	}

	uint32 Renderer2D::AssginTextureSlot(Texture* texture)
	{
		int32 textureIndex = -1;

		for (uint32 index = 0;
			 index < QuadData.CurrentTextureIndex;
			 index++)
		{
			//@InComplete: Right now we handle opengl only
			if (texture->OpenGL.TextureHandle == QuadData.TextureSlots[index])
			{
				textureIndex = index;
				break;
			}
		}

		if (textureIndex == -1)
		{
			if (QuadData.CurrentTextureIndex >= QuadData.MaxTextureSlots)
			{
				FlushQuadBatch();

				QuadData.CurrentTextureIndex = 0;

				memset(QuadData.TextureSlots,
					   0,
				       sizeof(uint32) * QuadData.MaxTextureSlots);
			}

			textureIndex = QuadData.CurrentTextureIndex;

			//@Incomplete: Right now we handle opengl only
			QuadData.TextureSlots[textureIndex] = texture->OpenGL.TextureHandle;
			QuadData.CurrentTextureIndex++;
		}

		Renderer::API.BindTexture(texture, textureIndex);

		return textureIndex;
	}

	glm::mat4 Renderer2D::CreateQuadTransform(const glm::vec3& position,
									          float32 rotationAngle,
									          const glm::vec2& scale)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
							  glm::rotate(glm::mat4(1.0f), rotationAngle, glm::vec3(0.0f, 0.0f, 1.0f)) *
							  glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, 1.0f));

		return transform;
	}

	glm::mat4 Renderer2D::CreateQuadTransform(const glm::vec3& position,
	                                          const glm::vec3& rotation,
	                                          const glm::vec3& scale)
	{
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
							  glm::toMat4(glm::quat(rotation)) *
							  glm::scale(glm::mat4(1.0f), scale);

		return transform;
	}

	void Renderer2D::SubmitQuadVertex(const QuadVertex& vertex)
	{
	   *QuadData.CurrentVertex = vertex;
		QuadData.CurrentVertex++;
	}

	void Renderer2D::FlushQuadBatch()
	{
		if (QuadData.Count > 0)
		{
			Renderer::API.BindVertexBuffer(&QuadData.VertexBuffer);
			Renderer::API.BindIndexBuffer(&QuadData.IndexBuffer);

			uint32 dataSize = 4 * sizeof(QuadVertex) * QuadData.Count;

			Renderer::API.SetVertexBufferData(&QuadData.VertexBuffer,
											  QuadData.VertexBase,
											  dataSize,
											  0);
 			Renderer::API.DrawIndexed16(&QuadData.VertexBuffer,
									    &QuadData.IndexBuffer,
 										QuadData.Count * 6);

			QuadData.CurrentVertex = QuadData.VertexBase;
			QuadData.Count = 0;
		}
	}

	SceneData Renderer2D::SceneData = {};
	QuadData  Renderer2D::QuadData = {};
}