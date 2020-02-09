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
		
		void SetVSync(bool enabled) override;
		void SwapBuffers() override;

	private:
		GLFWwindow* m_WindowHandle;
	};
}