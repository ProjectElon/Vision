#pragma once

#include "Vision/Platform/Window.h"

#include <memory>

namespace Vision
{
	class GraphicsContext
	{
	public:
		GraphicsContext(Window* window) {}
		virtual ~GraphicsContext() {}

		virtual void SetVSync(bool enabled) = 0;
		virtual void SwapBuffers() = 0;
		
		static Scope<GraphicsContext> Create(Window* window);
	};
}