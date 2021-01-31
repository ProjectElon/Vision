#pragma once

#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Renderer/Texture.h"

#define MAX_TEXTURE_SLOTS 128

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
		Texture WhitePixel;

		VertexBuffer VertexBuffer;
		IndexBuffer  IndexBuffer;

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
							   const glm::mat4& cameraProjection,
							   Shader* quadShader);

		static void EndScene();

		static void DrawQuad(const glm::vec3& position,
							 float32 rotationAngle,
							 const glm::vec2& scale,
							 const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void DrawQuad(const glm::mat4& transform,
							 const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void DrawTexture(const glm::vec3& position,
								float32 rotationAngle,
								const glm::vec2& scale,
								const Texture* texture,
								const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
								const glm::vec2& bottomLeftUV = glm::vec2(0.0f, 0.0f),
								const glm::vec2& topRightUV = glm::vec2(1.0f, 1.0f));

		static void DrawTexture(const glm::mat4& transform,
								const Texture* texture,
								const glm::vec4& color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f),
								const glm::vec2& bottomLeftUV = glm::vec2(0.0f, 0.0f),
								const glm::vec2& topRightUV = glm::vec2(1.0f, 1.0f));

		static void DrawString(Font* font,
		                       const std::string& text,
		                       float32 x,
		                       float32 y,
		                       const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f });

	private:
		static uint32 AssginTextureSlot(const Texture* texture);

		inline static glm::mat4 CreateQuadTransform(const glm::vec3& position,
										  		    float32 rotationAngle,
										  		    const glm::vec2& scale);

		static void SubmitQuadVertex(const QuadVertex& vertex);

		static void FlushQuadBatch();

	private:
		static SceneData s_SceneData;
		static QuadData  s_QuadData;
	};
}