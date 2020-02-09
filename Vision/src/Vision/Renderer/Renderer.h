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
		
		static void Initialize();
		static void Shutdown();

		inline static API GetAPI() { return s_API; }
		inline static RendererAPI& GetRendererAPI() { return *s_RendererAPI; }

	private:
		static RendererAPI* s_RendererAPI;
		static API s_API;
	};
}