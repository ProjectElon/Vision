#include "pch.h"
#include "OpenGLRendererAPI.h"

#include <glad/glad.h>

namespace Vision
{
	static void APIENTRY OpenGLDebugOutput(GLenum source,
										   GLenum type,
										   GLuint id,
										   GLenum severity,
										   GLsizei length,
										   const GLchar* message,
										   const void* userParam)
	{
		// ignore non-significant error/warning codes
		if (id == 131169 || id == 131185 || id == 131218 || id == 131204)
		{
			return;
		}

		VN_CORE_ERROR("[OPENGL ERROR MESSAGE]({0}) : {1}", id, message);

		const char* errorSource;

		switch (source)
		{
			case GL_DEBUG_SOURCE_API:			  { errorSource = "API";             } break;
			case GL_DEBUG_SOURCE_WINDOW_SYSTEM:   { errorSource = "Window System";   } break;
			case GL_DEBUG_SOURCE_SHADER_COMPILER: { errorSource = "Shader Compiler"; } break;
			case GL_DEBUG_SOURCE_THIRD_PARTY:     { errorSource = "Third Party";     } break;
			case GL_DEBUG_SOURCE_APPLICATION:     { errorSource = "Application";     } break;
			case GL_DEBUG_SOURCE_OTHER:           { errorSource = "Other";           } break;
		}

		const char* errorType;

		switch (type)
		{
			case GL_DEBUG_TYPE_ERROR:              { errorType = "Error";                 } break;
			case GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR:{ errorType =  "Deprecated Behaviour"; } break;
			case GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR: { errorType = "Undefined Behaviour";   } break;
			case GL_DEBUG_TYPE_PORTABILITY:        { errorType = "Portability";           } break;
			case GL_DEBUG_TYPE_PERFORMANCE:        { errorType = "Performance";           } break;
			case GL_DEBUG_TYPE_MARKER:             { errorType = "Marker";                } break;
			case GL_DEBUG_TYPE_PUSH_GROUP:         { errorType = "Push Group";            } break;
			case GL_DEBUG_TYPE_POP_GROUP:          { errorType = "Pop Group";             } break;
			case GL_DEBUG_TYPE_OTHER:              { errorType = "Other";                 } break;
		}

		const char* errorSeverity;

		switch (severity)
		{
			case GL_DEBUG_SEVERITY_HIGH:		{ errorSeverity = "high";         } break;
			case GL_DEBUG_SEVERITY_MEDIUM:      { errorSeverity = "medium";       } break;
			case GL_DEBUG_SEVERITY_LOW:         { errorSeverity = "low";          } break;
			case GL_DEBUG_SEVERITY_NOTIFICATION:{ errorSeverity = "notification"; } break;
		}

		VN_CORE_ERROR("[OPENGL] SOURCE : {0}\nTYPE : {1}\nSEVERITY : {2}", errorSource, errorType, errorSeverity);
	}

	OpenGLRendererAPI::OpenGLRendererAPI()
	{
	#ifndef VN_DIST
		
		int32_t flags;
		glGetIntegerv(GL_CONTEXT_FLAGS, &flags);

		if (flags & GL_CONTEXT_FLAG_DEBUG_BIT)
		{
			glEnable(GL_DEBUG_OUTPUT);
			glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
			glDebugMessageCallback(OpenGLDebugOutput, nullptr);
			glDebugMessageControl(GL_DONT_CARE, GL_DONT_CARE, GL_DONT_CARE, 0, nullptr, GL_TRUE);
		}

	#endif
	}

	void OpenGLRendererAPI::OnWindowResize(uint32_t width, uint32_t height) const
	{
		glViewport(0, 0, width, height);
	}

	void OpenGLRendererAPI::SetClearColor(const glm::vec4& color) const 
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void OpenGLRendererAPI::Clear(uint32_t flags) const
	{
		uint32_t glFlags = 0;
		
		if (flags & ClearColorBuffer)
		{
			glFlags |= GL_COLOR_BUFFER_BIT;
		}

		if (flags & ClearDepthBuffer)
		{
			glFlags |= GL_DEPTH_BUFFER_BIT;
		}

		if (flags & ClearStencilBuffer)
		{
			glFlags |= GL_STENCIL_BUFFER_BIT;
		}

		glClear(glFlags);
	}

	void OpenGLRendererAPI::DrawIndexed(const Ref<VertexBuffer>& vertexBuffer, const Ref<IndexBuffer>& indexBuffer, Primitive primitive) const
	{
		vertexBuffer->Bind();
		indexBuffer->Bind();

		uint32_t glPrimitive = 0;

		switch (primitive)
		{
			case Primitive::Points:    { glPrimitive = GL_POINTS;     } break;
			case Primitive::Lines:     { glPrimitive = GL_LINES;      } break;
			case Primitive::LineStrip: { glPrimitive = GL_LINE_STRIP; } break;
			case Primitive::Quads:     { glPrimitive = GL_QUADS;      } break;
			case Primitive::Triangles: { glPrimitive = GL_TRIANGLES;  } break;
		}

		glDrawElements(glPrimitive, indexBuffer->GetIndexCount(), GL_UNSIGNED_INT, (const void*)0);

		indexBuffer->UnBind();
		vertexBuffer->UnBind();
	}
}