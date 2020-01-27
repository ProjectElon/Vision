#pragma once

#include "Vision/Platform/Window.h"

#include <memory>

namespace Vision
{
	enum class RenderApi
	{
		OpenGL
	};

	class GraphicsContext
	{
	public:
		GraphicsContext(Window* window) {}
		virtual ~GraphicsContext() {}

		virtual void SwapBuffers() = 0;
		virtual void SetVSync(bool enabled) = 0;
		
		inline static void SetRenderApi(RenderApi renderApi) { s_RenderApi = renderApi; }

		inline static RenderApi GetRenderApi() { return s_RenderApi; };

		static std::unique_ptr<GraphicsContext> Create(Window* window);

	private:
		static RenderApi s_RenderApi;
	};
}