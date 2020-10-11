#pragma once

#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/VertexBuffer.h"
#include "Vision/Renderer/IndexBuffer.h"
#include "Vision/Renderer/Texture2D.h"
#include "Vision/Entity/Scene.h"

#define MAX_TEXTURE_SLOTS 64

namespace Vision
{
	struct SceneData
	{
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjection;	
	};
	
	struct QuadVertex
	{
		glm::vec2 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoord;
		float32   TextureIndex;
	};

	struct QuadData
	{
		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer>  IndexBuffer;

		QuadVertex* VertexBase    = nullptr;
		QuadVertex* CurrentVertex = nullptr;	
		
		uint32 MaxTextureSlots;
		uint32 CurrentTextureIndex = 0;
		uint32 TextureSlots[MAX_TEXTURE_SLOTS];
		int32  Samplers[MAX_TEXTURE_SLOTS];
		
		uint32 Count = 0;
		static const uint32 MaxCount = 10000;
	};
	
	class Renderer2D
	{
	public:
		static void Init();
		
		static void Shutdown();

		static void BeginScene(const glm::mat4& cameraTransform,
							   const OrthographicCameraComponent& camera,
							   const Ref<Shader>& quadShader);

		static void EndScene();
		
		static void DrawQuad(const glm::vec2& position,
							 float32 rotationAngle, 
							 const glm::vec2& scale,
							 const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void DrawQuad(const glm::mat3& transform,
							 const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void DrawTexture(const glm::vec2& position,
								float32 rotationAngle,
								const glm::vec2& scale,
								const Ref<Texture2D>& texture,
								const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float tilingFactor = 1.0f);

		static void DrawTexture(const glm::mat3& transform, 
								const Ref<Texture2D>& texture, 
								const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f), float tilingFactor = 1.0f);

		static void DrawSprite(const glm::vec2& position,
							   float32 rotationAngle,
							   const glm::vec2& scale,
							   const SpriteRendererComponent& sprite);

		static void DrawSprite(const glm::mat3& transform, const SpriteRendererComponent& sprite);
		static void DrawSprite(const glm::mat4& transform, const SpriteRendererComponent& sprite);

	private:
		static void InitQuadSetup();

		static uint32 AssginTextureSlot(const Ref<Texture2D>& texture);

		inline static glm::mat3 CreateQuadTransform(const glm::vec2& position,
										  		    float32 rotationAngle,
										  		    const glm::vec2& scale);
		
		static void SubmitQuadVertex(const QuadVertex& vertex);
		
		static void FlushQuadBatch();
		
	private:
		static SceneData      s_SceneData;
		static QuadData       s_QuadData;
		static Ref<Texture2D> s_WhitePixel;
	};
}