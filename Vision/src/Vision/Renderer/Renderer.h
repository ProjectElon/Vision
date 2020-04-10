#pragma once

#include "Vision/Renderer/RendererAPI.h"

namespace Vision
{
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
		
		inline static RendererAPI& GetRendererAPI()
		{ 
			return *s_RendererAPI;
		}
		
		static void Init();
		static void Shutdown();

		static void OnWindowResize(uint32_t width, uint32_t height);

	private:
		static API s_API;
		static Scope<RendererAPI> s_RendererAPI;
	};
}