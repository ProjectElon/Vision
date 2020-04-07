#pragma once

#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/VertexBuffer.h"
#include "Vision/Renderer/IndexBuffer.h"
#include "Vision/Renderer/Texture2D.h"
#include "Vision/Renderer/OrthographicCamera.h"
#include "Vision/Renderer/Sprite.h"

namespace Vision
{
	struct SceneData
	{
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjection;
	};

	struct QuadVertex
	{
		glm::vec3 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoord;
		float TextureIndex;
	};

	struct RendererData
	{
		Ref<Texture2D> WhitePixel;

		Ref<VertexBuffer> QuadVertexBuffer;
		Ref<IndexBuffer> QuadIndexBuffer;
		Ref<Shader> QuadShader;

		QuadVertex* QuadVertexBase = nullptr;
		QuadVertex* CurrentQuadVertex = nullptr;
		uint32_t QuadCount = 0;
		
		uint32_t MaxTextureSlots;
		std::unordered_map<Ref<Texture2D>, uint32_t> TextureSlots;
		uint32_t CurrentTextureIndex = 1;
		int32_t* Samplers = nullptr;

		const uint32_t MaxQuadCount = 10000;
		const uint32_t MaxQuadVertexCount = MaxQuadCount * 4;
		const uint32_t MaxQuadIndexCount = MaxQuadCount * 6;
	};

	class Renderer2D
	{
	public:
		static void Init();
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera, const Ref<Shader>& shader);
		static void EndScene();
		
		static void DrawQuad(const glm::vec3& position, float rotation, const glm::vec2& scale, const glm::vec4& color);
		static void DrawTexture(const glm::vec3& position, float rotation, const glm::vec2& scale, const Ref<Texture2D>& texture, const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
		static void DrawSprite(const glm::vec3& position, float rotation, const glm::vec2& scale, const Ref<Sprite>& sprite);

	private:
		static void SubmitQuadVertex(const QuadVertex& quadVertex);
		static void Flush();
		
	private:
		static SceneData s_SceneData;
		static RendererData s_Data;
	};
}