#pragma once

#include "Vision/Core/Common.h"

namespace Vision
{
	enum class ShaderDataType
	{
		Bool,
		Int8,
		UInt8,
		Int16,
		UInt16,
		Int32,
		UInt32,
		Float,
		Float2,
		Float3,
		Float4,
		Matrix3,
		Matrix4
	};

	enum ClearFlags
	{
		ClearColorBuffer   = VnBitUInt32(1),
		ClearDepthBuffer   = VnBitUInt32(2),
		ClearStencilBuffer = VnBitUInt32(3)
	};

	enum class Primitive
	{
		Triangles
	};

	struct Window;
	struct VertexBuffer;
	struct IndexBuffer;

	class Renderer
	{
	public:
		static uint32 ViewportWidth;
		static uint32 ViewportHeight;

		static Window* RenderTargetWindow;

		static void Init(Window* renderTargetWindow);
		static void Shutdown();

		static void Clear(ClearFlags clearflags);

		static void SetClearColor(float32 r,
								  float32 g,
								  float32 b,
								  float32 a);

		static void SetViewport(uint32 x,
								uint32 y,
								uint32 width,
								uint32 height);

		static void DrawIndexed(const VertexBuffer* vertexBuffer,
								const IndexBuffer* indexBuffer,
								uint32 count,
								Primitive primitive = Primitive::Triangles);

		static void SetVSync(bool enabled);
		static void SwapBuffers();

		static int32 GetMaxTextureSlots();
	};
}