#pragma once

#include "Vision/Renderer/RendererAPI.h"
#include "Vision/Renderer/Texture.h"
#include "Vision/Renderer/OrthographicCamera.h"

namespace Vision
{
	struct SceneData
	{
		glm::vec3 CameraPosition;
		glm::mat4 ViewProjection;
	};

	struct Vertex2D
	{
		glm::vec3 Position;
		glm::vec2 TexCoord;
	};

	class Renderer
	{
	public:
		enum class API : uint8_t
		{
			None,
			OpenGL
		};

	public:
		inline static void SetAPI(API api) { s_API = api; }
		inline static API GetAPI() { return s_API; }
		
		inline static RendererAPI& GetRendererAPI() { return *s_RendererAPI; }
		
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

		static void BeginScene2D(const OrthographicCamera& camera);
		
		static void DrawSprite(const Ref<Shader>& shader, 
							   const glm::vec3& position, 
							   float rotation, 
							   const glm::vec2& scale, 
							   const Ref<Texture>& sprite, 
							   const glm::vec4& tint = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));

		static void DrawQuad(const Ref<Shader>& shader,
							 const glm::vec3& position,
							 float rotation,
							 const glm::vec2& scale,
							 const glm::vec4& color);

		static void EndScene2D();

	private:
		static API s_API;
		static Scope<RendererAPI> s_RendererAPI;
		
		static SceneData s_SceneData2D;
		
		static Ref<VertexBuffer> s_QuadVB;
		static Ref<IndexBuffer>  s_QuadIB;
		static Ref<Texture> s_WhitePixel;
	};
}