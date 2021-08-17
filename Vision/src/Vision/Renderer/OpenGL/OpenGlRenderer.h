#include "Vision/Core/Defines.h"
#include "Vision/Renderer/Renderer.h"

#ifdef VN_RENDERER_API_OPENGL

namespace Vision
{
	void InitializeOpenGL(RendererAPI* rendererAPI);
	void UninitializeOpenGL();

	void OpenGLSetClearColor(float32 r, float32 g, float32 b, float32 a);
	void OpenGLClear(uint32 clearFlags);

	void OpenGLSetViewport(const struct ViewportRect* viewportRect);

	void OpenGLDrawIndexed16(const struct VertexBuffer* vertexBuffer,
							 const struct IndexBuffer* indexBuffer,
							 uint32 count);

	void OpenGLDrawIndexed32(const struct VertexBuffer* vertexBuffer,
							 const struct IndexBuffer* indexBuffer,
							 uint32 count);

	void OpenGLSetVSync(bool enabled);
	void OpenGLSwapBuffers();
}

#endif