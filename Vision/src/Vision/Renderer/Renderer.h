#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Renderer/RendererTypes.h"

#include "Vision/Renderer/Texture.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/Buffers.h"
#include "Vision/Renderer/FrameBuffer.h"

namespace Vision
{
	struct RendererAPI
	{
		// Pipeline State Functions
		void (*SetClearColor) (float32 r, float32 g, float32 b, float32 a);
		void (*Clear) (uint32 clearFlags);
		void (*SetViewport) (const ViewportRect* viewportRect);
		void (*DrawIndexed16) (const VertexBuffer* vertexBuffer,
	                           const IndexBuffer* indexBuffer,
							   uint32 count);

		void (*DrawIndexed32) (const VertexBuffer* vertexBuffer,
							   const IndexBuffer* indexBuffer,
							   uint32 count);
		void (*SetVSync) (bool enabled);
		void (*SwapBuffers) ();

		// Buffers
		void (*InitVertexBuffer)(VertexBuffer* vertexBuffer,
		                         const void* data,
		                         uint32 sizeInBytes,
		                         BufferUsage usage);

		void (*UninitVertexBuffer)(VertexBuffer* vertexBuffer);

		void (*SetVertexBufferLayout)(VertexBuffer* vertexBuffer,
		                              VertexLayout* vertexLayout);

		void (*SetVertexBufferData)(VertexBuffer* vertexBuffer,
		                            const void* data,
		                            uint32 sizeInBytes,
		                            uint32 offset);

		void (*BindVertexBuffer)(VertexBuffer* vertexBuffer);

		void (*InitIndexBuffer16)(IndexBuffer* indexBuffer,
		                          const uint16* data,
		                          uint32 sizeInBytes,
				  				  BufferUsage usage);

		void (*InitIndexBuffer32)(IndexBuffer* indexBuffer,
		                          const uint32* data,
					  			  uint32 sizeInBytes,
					  			  BufferUsage usage);

		void (*UninitIndexBuffer)(IndexBuffer* indexBuffer);

		void (*BindIndexBuffer)(IndexBuffer* indexBuffer);

		// Texture Functions
		void (*InitTexture)(Texture* texture,
		                    void* pixels,
		                    uint32 width,
		                    uint32 height,
		                    PixelFormat pixelFormat,
		                    WrapMode wrapModeX,
		                    WrapMode wrapModeY,
		                    FilterMode filter);

		void (*UninitTexture)(Texture* texture);
		void (*BindTexture)(Texture* texture, uint32 textureSlot);

		void (*SetTextureWrapMode)(Texture* texture,
		                           WrapMode wrapModeX,
		                           WrapMode wrapModeY);

		void (*SetTextureFilterMode)(Texture* texture, FilterMode filter);

		// Shader Functions
		void (*InitShader)(Shader* shader, const std::string& filePath);
		void (*UninitShader)(Shader* shader);

		void (*BindShader)(Shader* shader);

		void (*SetUniformInt)(Shader* shader, const char* uniform, int32 value);

		void (*SetUniformIntArray)(Shader* shader,
		                           const char* uniform,
		                           const int32* values,
		                           uint32 count);

		void (*SetUniformFloat)(Shader* shader,
		                        const char* uniform,
		                        float32 value);

		void (*SetUniformFloat2)(Shader* shader,
		                         const char* uniform,
		                         const float32* values);

		void (*SetUniformFloat3)(Shader* shader,
		                         const char* uniform,
		                         const float32* values);

		void (*SetUniformFloat4)(Shader* shader,
		                         const char* uniform,
		                         const float32* values);

		void (*SetUniformMatrix3)(Shader* shader,
		                          const char* uniform,
		                          const float32* matrix3);

		void (*SetUniformMatrix4)(Shader* shader,
		                          const char* uniform,
		                          const float32* matrix4);

		// FrameBuffer Functions
		void (*InitFrameBuffer)(FrameBuffer* frameBuffer,
		                        const FrameBufferAttachmentSpecification& specification,
                                uint32 width,
                                uint32 height);

		void (*UninitFrameBuffer)(FrameBuffer* frameBuffer);

		void (*ResizeFrameBuffer)(FrameBuffer* frameBuffer,

		                          uint32 width,
                                  uint32 height);

		int32 (*ReadPixel)(FrameBuffer* frameBuffer,
                          uint32 attachmentIndex,
                          int32 x,
                          int32 y);

		void (*ClearColorAttachment)(FrameBuffer* frameBuffer,
                                     uint32 attachmentIndex,
                                     const void* value);

		void (*BindFrameBuffer)(FrameBuffer* frameBuffer);
		void (*UnbindFrameBuffer)(FrameBuffer* frameBuffer);
	};

	struct Renderer
	{
		static RendererAPI API;
		static ViewportRect ViewportRect;
		static RendererCapabilites Capabilites;

		static void Initialize();
		static void Shutdown();
	};
}