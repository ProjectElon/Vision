#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Renderer/RendererTypes.h"

namespace Vision
{
	struct Texture;
	struct Shader;
	struct VertexBuffer;
	struct IndexBuffer;
	struct FrameBuffer;
	
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

		void* (*ConvertTextureToImGuiTexture)(Texture *texture);
	};

	struct Renderer
	{
		static RendererAPI 		   API;
		static ViewportRect 	   ViewportRect;
		static RendererCapabilites Capabilites;

		static void Initialize();
		static void Shutdown();

		inline static void SetClearColor(float32 r, float32 g, float32 b, float32 a)
		{
			API.SetClearColor(r, g, b, a);
		}

		inline static void Clear(uint32 clearFlags)
		{
			API.Clear(clearFlags);
		}

		inline static void SetViewport(const struct ViewportRect* viewportRect)
		{
			API.SetViewport(viewportRect);
		}

		inline static void DrawIndexed16(const VertexBuffer *vertexBuffer,
		                                 const IndexBuffer *indexBuffer,
		                                 uint32 count)
		{
			API.DrawIndexed16(vertexBuffer, indexBuffer, count);
		}

		inline static void DrawIndexed32(const VertexBuffer *vertexBuffer,
		                                 const IndexBuffer *indexBuffer,
		                                 uint32 count)
		{
			API.DrawIndexed32(vertexBuffer, indexBuffer, count);
		}

		inline static void SetVSync(bool enabled)
		{
			API.SetVSync(enabled);
		}

		inline static void SwapBuffers()
		{
			API.SwapBuffers();
		}

		inline static void InitVertexBuffer(VertexBuffer* vertexBuffer,
		                         		    const void* data,
		                         			uint32 sizeInBytes,
		                         			BufferUsage usage)
		{
			API.InitVertexBuffer(vertexBuffer, data, sizeInBytes, usage);
		}

		inline static void UninitVertexBuffer(VertexBuffer* vertexBuffer)
		{
			API.UninitVertexBuffer(vertexBuffer);
		}

		inline static void SetVertexBufferLayout(VertexBuffer* vertexBuffer,
		                              			 VertexLayout* vertexLayout)
		{
			API.SetVertexBufferLayout(vertexBuffer, vertexLayout);
		}

		inline static void SetVertexBufferData(VertexBuffer* vertexBuffer,
		                            		   const void* data,
		                            		   uint32 sizeInBytes,
		                            		   uint32 offset)
		{
			API.SetVertexBufferData(vertexBuffer, data, sizeInBytes, offset);
		}

		inline static void BindVertexBuffer(VertexBuffer* vertexBuffer)
		{
			API.BindVertexBuffer(vertexBuffer);
		}

		inline static void InitIndexBuffer16(IndexBuffer* indexBuffer,
		                          			 const uint16* data,
		                          			 uint32 sizeInBytes,
				  				  			 BufferUsage usage)
		{
			API.InitIndexBuffer16(indexBuffer, data, sizeInBytes, usage);
		}

		inline static void InitIndexBuffer32(IndexBuffer* indexBuffer,
		                          			 const uint32* data,
					  			  			 uint32 sizeInBytes,
					  			  			 BufferUsage usage)
		{
			API.InitIndexBuffer32(indexBuffer, data, sizeInBytes, usage);
		}

		inline static void UninitIndexBuffer(IndexBuffer* indexBuffer)
		{
			API.UninitIndexBuffer(indexBuffer);
		}

		inline static void BindIndexBuffer(IndexBuffer* indexBuffer)
		{
			API.BindIndexBuffer(indexBuffer);
		}

		inline static void InitTexture(Texture* texture,
		                    		   void* pixels,
		                    		   uint32 width,
		                    		   uint32 height,
		                    		   PixelFormat pixelFormat,
		                    		   WrapMode wrapModeX,
		                    		   WrapMode wrapModeY,
		                    		   FilterMode filter)
		{
			API.InitTexture(texture,
			                pixels,
			                width,
			                height,
			                pixelFormat,
			                wrapModeX,
			                wrapModeY,
			                filter);
		}

		inline static void UninitTexture(Texture* texture)
		{
			API.UninitTexture(texture);
		}

		inline static void BindTexture(Texture* texture, uint32 textureSlot)
		{
			API.BindTexture(texture, textureSlot);
		}

		inline static void SetTextureWrapMode(Texture* texture,
		                           			  WrapMode wrapModeX,
		                           			  WrapMode wrapModeY)
		{
			API.SetTextureWrapMode(texture, wrapModeX, wrapModeY);
		}

		inline static void SetTextureFilterMode(Texture* texture, FilterMode filter)
		{
			API.SetTextureFilterMode(texture, filter);
		}

		// Shader Functions
		inline static void InitShader(Shader* shader, const std::string& filePath)
		{
			API.InitShader(shader, filePath);
		}

		inline static void UninitShader(Shader* shader)
		{
			API.UninitShader(shader);
		}

		inline static void BindShader(Shader* shader)
		{
			API.BindShader(shader);
		}

		inline static void SetUniformInt(Shader* shader, const char* uniform, int32 value)
		{
			API.SetUniformInt(shader, uniform, value);
		}

		inline static void SetUniformIntArray(Shader* shader,
		                           			  const char* uniform,
		                           			  const int32* values,
		                           			  uint32 count)
		{
			API.SetUniformIntArray(shader, uniform, values, count);
		}

		inline static void SetUniformFloat(Shader* shader,
		                        		   const char* uniform,
		                        		   float32 value)
		{
			API.SetUniformFloat(shader, uniform, value);
		}

		inline static void SetUniformFloat2(Shader* shader,
		                         			const char* uniform,
		                         			const float32* values)
		{
			API.SetUniformFloat2(shader, uniform, values);
		}

		inline static void SetUniformFloat3(Shader* shader,
		                         			const char* uniform,
		                         			const float32* values)
		{
			API.SetUniformFloat3(shader, uniform, values);
		}

		inline static void SetUniformFloat4(Shader* shader,
		                         			const char* uniform,
		                         			const float32* values)
		{
			API.SetUniformFloat4(shader, uniform, values);
		}

		inline static void SetUniformMatrix3(Shader* shader,
		                          			 const char* uniform,
		                          			 const float32* matrix3)
		{
			API.SetUniformMatrix3(shader, uniform, matrix3);
		}

		inline static void SetUniformMatrix4(Shader* shader,
		                          			 const char* uniform,
		                          			 const float32* matrix4)
		{
			API.SetUniformMatrix4(shader, uniform, matrix4);
		}

		// FrameBuffer Functions
		inline static void InitFrameBuffer(FrameBuffer* frameBuffer,
		                        		   const FrameBufferAttachmentSpecification& specification,
                                		   uint32 width,
                                		   uint32 height)
		{
			API.InitFrameBuffer(frameBuffer, specification, width, height);
		}

		inline static void UninitFrameBuffer(FrameBuffer* frameBuffer)
		{
			API.UninitFrameBuffer(frameBuffer);
		}

		inline static void ResizeFrameBuffer(FrameBuffer* frameBuffer,
		                          			 uint32 width,
                                  			 uint32 height)
		{
			API.ResizeFrameBuffer(frameBuffer, width, height);
		}

		inline static int32 ReadPixel(FrameBuffer* frameBuffer,
                           			  uint32 attachmentIndex,
                           			  int32 x,
                           			  int32 y)
		{
			return API.ReadPixel(frameBuffer, attachmentIndex, x, y);
		}

		inline static void ClearColorAttachment(FrameBuffer* frameBuffer,
                                     			uint32 attachmentIndex,
                                     			const void* value)
		{
			API.ClearColorAttachment(frameBuffer, attachmentIndex, value);
		}

		inline static void BindFrameBuffer(FrameBuffer* frameBuffer)
		{
			API.BindFrameBuffer(frameBuffer);
		}

		inline static void UnbindFrameBuffer(FrameBuffer* frameBuffer)
		{
			API.UnbindFrameBuffer(frameBuffer);
		}

		inline static void* ConvertTextureToImGuiTexture(Texture* texture)
		{
			return API.ConvertTextureToImGuiTexture(texture);
		}
	};
}