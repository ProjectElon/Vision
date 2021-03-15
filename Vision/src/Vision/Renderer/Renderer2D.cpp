#include "pch.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/Renderer2D.h"
#include "Vision/Entity/Scene.h"

#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/matrix_transform_2d.hpp>
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>

namespace Vision
{
	void Renderer2D::Init()
	{
		uint32 white = 0xffffffff;

		s_QuadData.WhitePixel.Init(&white, 1, 1, PixelFormat::RGBA);

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
			{ ShaderDataType::Float3, "a_Position",     false },
			{ ShaderDataType::Float4, "a_Color", 	    false },
			{ ShaderDataType::Float2, "a_TextureCoord", false },
			{ ShaderDataType::UInt32, "a_TextureIndex", false },
			{ ShaderDataType::Int32,  "a_EntityIndex",  false },
		});

		s_QuadData.VertexBase = new QuadVertex[4 * s_QuadData.MaxCount];
		s_QuadData.CurrentVertex = s_QuadData.VertexBase;

		s_QuadData.VertexBuffer.Init(nullptr,
									 4 * sizeof(QuadVertex) * s_QuadData.MaxCount,
									 BufferUsage::Dynamic);

		s_QuadData.VertexBuffer.SetLayout(quadVertexlayout);

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

		s_QuadData.IndexBuffer.Init(indices, maxQuadIndexCount * sizeof(uint32));

		delete[] indices;
	}

	void Renderer2D::Shutdown()
	{
		delete[] s_QuadData.VertexBase;
	}

	void Renderer2D::BeginScene(const glm::mat4& cameraView,
								const glm::mat4& cameraProjection,
								Shader* shader)
	{
		s_SceneData.ViewProjection = cameraProjection * cameraView;

		shader->Bind();
		shader->SetUniformIntArray("u_Textures",
										s_QuadData.Samplers,
										s_QuadData.MaxTextureSlots);

		shader->SetUniformMatrix4("u_ViewProj",
								   glm::value_ptr(s_SceneData.ViewProjection));
	}

	void Renderer2D::DrawQuad(const glm::mat4& transform,
						      const glm::vec4& color,
						      int32 entityIndex)
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
								 const Texture* texture,
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
								 const Texture* texture,
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
								 const Texture* texture,
								 const glm::vec4& color,
								 const glm::vec2& bottomLeftUV,
								 const glm::vec2& topRightUV,
								 int32 entityIndex)
	{
		if (s_QuadData.Count >= s_QuadData.MaxCount)
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

		s_QuadData.Count++;
	}

	void Renderer2D::DrawString(BitmapFont* font,
	                            const std::string& text,
	                            glm::vec2 position,
	                            const glm::vec4& color,
	                            int32 entityIndex)
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

	void Renderer2D::DrawString(BitmapFont* font,
		                   		const std::string& text,
		                   		const glm::vec3& position,
		                   		const glm::vec4& color,
		                   		int32 entityIndex)
	{
		EditorState& editorState = Scene::EditorState;
		PerspectiveCamera& sceneCamera = editorState.SceneCamera;

		glm::vec4 p(position.x, position.y, position.z, 1.0f);

		p = sceneCamera.Projection * sceneCamera.View * p;
		p = p / p.w;

		float32 halfWidth = static_cast<float32>(Renderer::ViewportWidth) / 2.0f;
		float32 halfHeight = static_cast<float32>(Renderer::ViewportHeight) / 2.0f;
		float32 height = static_cast<float32>(Renderer::ViewportHeight);

		p.x = (p.x + 1) * halfWidth;
		p.y = (p.y + 1) * halfHeight;
		p.y = height - p.y; // positive y is down coords

		glm::vec2 pp(p.x, p.y);
		DrawString(font, text, pp, color, entityIndex);
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

		texture->Bind(textureIndex);

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

	glm::mat4 Renderer2D::CreateQuadTransform(const glm::vec3& position,
	                                          const glm::vec3& rotation,
	                                          const glm::vec3& scale)
	{
		glm::quat rot  = glm::quat(rotation);
		glm::mat4 transform = glm::translate(glm::mat4(1.0f), position) *
							  glm::toMat4(rot) *
							  glm::scale(glm::mat4(1.0f), scale);

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
			s_QuadData.VertexBuffer.Bind();
			s_QuadData.IndexBuffer.Bind();

			uint32 dataSize = 4 * sizeof(QuadVertex) * s_QuadData.Count;

			s_QuadData.VertexBuffer.SetData(s_QuadData.VertexBase,
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