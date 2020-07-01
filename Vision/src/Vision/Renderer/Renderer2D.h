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
		glm::vec2 Position;
		glm::vec4 Color;
		glm::vec2 TextureCoord;
		real32    TextureIndex;
	};

	struct QuadData
	{
		Ref<Texture2D> WhitePixel;

		Ref<VertexBuffer> VertexBuffer;
		Ref<IndexBuffer>  IndexBuffer;

		QuadVertex* VertexBase    = nullptr;
		QuadVertex* CurrentVertex = nullptr;	
		
		uint32 MaxTextureSlots;
		uint32* TextureSlots;
		uint32 CurrentTextureIndex = 0;
		int32* Samplers = nullptr;
		
		uint32 Count = 0;
		static const uint32 MaxCount = 10000;
	};
	
	class Renderer2D
	{
	public:
		static void Init();
		
		static void Shutdown();

		static void BeginScene(const OrthographicCamera& camera,
							   const Ref<Shader>& quadShader);

		static void EndScene();
		
		static void DrawQuad(const glm::vec2& position,
							 real32 rotationAngle, 
							 const glm::vec2& scale,
							 const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void DrawTexture(const glm::vec2& position,
								real32 rotationAngle,
								const glm::vec2& scale,
								const Ref<Texture2D>& texture,
								const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
							
		static void DrawSprite(const glm::vec2& position,
							   real32 rotationAngle,
							   const glm::vec2& scale,
							   const Ref<Sprite>& sprite);

	private:
		static void InitQuadSetup();

		static uint32 AssginTextureSlot(const Ref<Texture2D>& texture);

		inline static glm::mat3 CreateQuadTransform(const glm::vec2& position,
										  		    real32 rotationAngle,
										  		    const glm::vec2& scale);
		
		static void SubmitQuadVertex(const QuadVertex& vertex);
		
		static void FlushQuadBatch();
		
	private:
		static SceneData s_SceneData;
		static QuadData  s_QuadData;
	};
}