#pragma once

#include "Vision/Renderer/GraphicsContext.h"

struct GLFWwindow;

namespace Vision
{
	class DesktopOpenGLContext : public GraphicsContext
	{
	public:
		DesktopOpenGLContext(Window* window);
		virtual ~DesktopOpenGLContext() {}
		
		void SwapBuffers() override;
		void SetVSync(bool enabled) override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}