#pragma once

#include <glm/glm.hpp>

#include "Vision/Renderer/RendererTypes.h"

#define MAX_TEXTURE_SLOTS 128

namespace Vision
{
	struct Shader;
	struct VertexBuffer;
	struct IndexBuffer;
	struct Font;
	struct BitmapFont;

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

		uint32 TextureIndex;
		int32 EntityIndex;
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
		static SceneData SceneData;
		static QuadData  QuadData;

		static void Initialize();
		static void Shutdown();

		static void BeginScene(const glm::mat4& cameraTransform,
							   const glm::mat4& cameraProjection,
							   Shader* quadShader);

		static void EndScene();

		static void DrawQuad(const glm::vec3& position,
							 float32 rotationAngle,
							 const glm::vec2& scale,
							 const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
							 int32 entityIndex = 0);

		static void DrawQuad(const glm::mat4& transform,
							 const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
							 int32 entityIndex = 0);

		static void DrawTexture(const glm::vec3& position,
								float32 rotationAngle,
								const glm::vec2& scale,
								Texture* texture,
								const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
								const glm::vec2& bottomLeftUV = { 0.0f, 0.0f },
								const glm::vec2& topRightUV = { 1.0f, 1.0f },
								int32 entityIndex = 0);

		static void DrawTexture(const glm::vec3& position,
								const glm::vec3& rotation,
								const glm::vec3& scale,
								Texture* texture,
								const glm::vec4& color,
								const glm::vec2& bottomLeftUV,
								const glm::vec2& topRightUV,
								int32 entityIndex);

		static void DrawTexture(const glm::mat4& transform,
								Texture* texture,
								const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
								const glm::vec2& bottomLeftUV = { 0.0f, 0.0f },
								const glm::vec2& topRightUV = { 1.0f, 1.0f },
								int32 entityIndex = 0);

		static void DrawString(BitmapFont* font,
		                       const std::string& text,
		                       glm::vec2 position,
		                       const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
							   int32 entityIndex = 0);

		static void DrawString(BitmapFont* font,
		                       const std::string& text,
		                       const glm::vec3& position,
		                       const glm::vec4& color = { 1.0f, 1.0f, 1.0f, 1.0f },
		                       int32 entityIndex = 0);

	private:
		static uint32 AssginTextureSlot(Texture* texture);

		static glm::mat4 CreateQuadTransform(const glm::vec3& position,
										  	 float32 rotationAngle,
										     const glm::vec2& scale);

		static glm::mat4 CreateQuadTransform(const glm::vec3& position,
		                                     const glm::vec3& rotation,
		                                     const glm::vec3& scale);

		static void SubmitQuadVertex(const QuadVertex& vertex);

		static void FlushQuadBatch();
	};
}