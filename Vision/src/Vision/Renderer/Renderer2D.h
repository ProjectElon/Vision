#pragma once

#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Renderer/Texture.h"
#include "Vision/Renderer/OrthographicCamera.h"

namespace Vision
{
	class Renderer2D
	{
	public:
		struct SceneData
		{
			glm::vec3 CameraPosition;
			glm::mat4 ViewProjection;
		};

		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		struct BatchedQuadVertex
		{
			glm::vec3 Position;
			glm::vec4 Color;
			glm::vec2 TexCoord;
			float TexID;
		};

	public:
		static void Init();
		static void Shutdown();
		
		static void BeginScene(const OrthographicCamera& camera);

		static void DrawQuad(const Ref<Shader>& shader, const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture>& texture, const glm::vec4& tint = s_RendererData.WhiteColor);
		static void DrawQuad(const Ref<Shader>& shader, const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color);
		
		static void EndScene();

		static void BeginBatchedScene(const Ref<Shader>& shader, const OrthographicCamera& camera);
		
		static void SubmitQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const Ref<Texture>& texture, const glm::vec4& tint = s_RendererData.WhiteColor);
		static void SubmitQuad(const glm::vec3& position, float rotation, const glm::vec2& size, const glm::vec4& color);

		static void EndBatchedScene();

	private:
		static void InitQuadResources();
		static void InitBatchedQuadResources();
		
		static void SubmitQuadVertex(const glm::vec3& position, const glm::vec2& texCoord, float texID, const glm::vec4& color);
		
		static void FlushBatch();
		
	private:
		struct RendererData
		{
			Ref<Texture> WhitePixel;
			glm::vec4 WhiteColor;

			Ref<VertexBuffer> QuadVB;
			Ref<IndexBuffer>  QuadIB;

			Ref<VertexBuffer> BatchedQuadVB;
			Ref<IndexBuffer> BatchedQuadIB;		

			uint32_t CurrentBatchedQuadCount = 0;

			BatchedQuadVertex* QuadVertexPtr;
			BatchedQuadVertex* CurrentQuadVertexPtr;

			Ref<Shader> BatchedSpriteShader;

			uint32_t CurrentTextureSlot = 1;
			std::unordered_map<std::string, uint32_t> TextureSlots;
		};

		static SceneData s_SceneData;
		static RendererData s_RendererData;
		
		static const uint32_t s_MaxBatchedQuadCount = 50000;
		static const uint32_t s_MaxTextureSlots = 32;
	};
}