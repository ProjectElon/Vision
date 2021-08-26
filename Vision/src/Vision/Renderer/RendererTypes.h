#pragma once

#include "Vision/Core/Defines.h"

#ifdef VN_RENDERER_API_OPENGL
    #include "Vision/Renderer/OpenGL/OpenGLRendererTypes.h"
#endif

#include <vector>
#include <glm/glm.hpp>

namespace Vision
{
    struct ViewportRect
    {
        uint32 X;
        uint32 Y;
        uint32 Width;
        uint32 Height;
    };

    enum ClearFlags : uint32
    {
        ClearFlags_Color   = VnBitU32Mask(0),
        ClearFlags_Depth   = VnBitU32Mask(1),
        ClearFlags_Stencil = VnBitU32Mask(2)
    };

    struct RendererCapabilites
    {
        int32 MaxTextureSlots;
    };

    enum WrapMode : uint8
    {
        WrapMode_Repeat = 0,
        WrapMode_ClampToEdge = 1,
    };

    enum FilterMode : uint8
    {
        FilterMode_Point = 0,
        FilterMode_Bilinear = 1
    };

    enum PixelFormat : uint8
    {
        PixelFormat_R    = 0,
        PixelFormat_RGB  = 1,
        PixelFormat_RGBA = 2
    };

    struct TextureRect
    {
        int32 X = 0;
        int32 Y = 0;

        uint32 Width = 0;
        uint32 Height = 0;
    };

    struct UVRect
    {
        glm::vec2 BottomLeft;
        glm::vec2 TopRight;
    };

    struct Texture
    {
        union
        {
#ifdef VN_RENDERER_API_OPENGL
            OpenGLTexture OpenGL;
#endif
        };

        uint32 Width = 0;
        uint32 Height = 0;

        WrapMode WrapX = WrapMode_Repeat;
        WrapMode WrapY = WrapMode_Repeat;

        FilterMode  Filter = FilterMode_Bilinear;
        PixelFormat Format = PixelFormat_RGBA;
    };

    enum ShaderDataType : uint8
    {
        ShaderDataType_Bool,
        ShaderDataType_Int8,
        ShaderDataType_UInt8,
        ShaderDataType_Int16,
        ShaderDataType_UInt16,
        ShaderDataType_Int32,
        ShaderDataType_UInt32,
        ShaderDataType_Float,
        ShaderDataType_Float2,
        ShaderDataType_Float3,
        ShaderDataType_Float4,
        ShaderDataType_Matrix3,
        ShaderDataType_Matrix4
    };

    enum ShaderType : uint8
    {
        ShaderType_Vertex = 0,
        ShaderType_Fragment = 1,
        ShaderType_Geometry = 2,
        ShaderType_Compute = 3,
        ShaderType_Count = 4,
        ShaderType_None = 5
    };

    struct Shader
    {
        union
        {
#ifdef VN_RENDERER_API_OPENGL
            OpenGLShader OpenGL;
#endif
        };
    };

    enum FrameBufferTextureFormat : uint8
    {
        FrameBufferTextureFormat_None = 0,

        FrameBufferTextureFormat_RGBA8,

        FrameBufferTextureFormat_Depth24Stencil8,
        FrameBufferTextureFormat_Depth = FrameBufferTextureFormat_Depth24Stencil8,

        FrameBufferTextureFormat_RedInt32,
        FrameBufferTextureFormat_RedUInt32
    };

    struct FrameBufferTextureSpecification
    {
        FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat_None;

        WrapMode WrapX = WrapMode_ClampToEdge;
        WrapMode WrapY = WrapMode_ClampToEdge;
        FilterMode FilterMode = FilterMode_Point;

        FrameBufferTextureSpecification() = default;

        FrameBufferTextureSpecification(const FrameBufferTextureFormat& textureFormat)
            : TextureFormat(textureFormat)
        {}
    };

    struct FrameBufferAttachmentSpecification
    {
        std::vector<FrameBufferTextureSpecification> Attachments;

        FrameBufferAttachmentSpecification() = default;
        FrameBufferAttachmentSpecification(const std::initializer_list<FrameBufferTextureSpecification>& attachments)
            : Attachments(attachments)
        {}
    };

    struct FrameBuffer
    {
        union
        {
#ifdef VN_RENDERER_API_OPENGL
            OpenGLFrameBuffer OpenGL;
#endif
        };

        uint32 Width;
        uint32 Height;

        uint32 ColorAttachmentCount;
        FrameBufferTextureSpecification ColorAttachmentSpecification[MaxColorAttachments];
        FrameBufferTextureSpecification DepthAttachmentSpecification;
    };

    struct VertexAttribute
    {
        ShaderDataType Type;
        const char*    Name;
        bool           Normalized;
    };

    struct VertexLayout
    {
        std::vector<VertexAttribute> Attributes;

        VertexLayout() = default;

        VertexLayout(const std::initializer_list<VertexAttribute>& attributes)
            : Attributes(attributes)
        {}
    };

    enum class BufferUsage : uint8
    {
        Static  = 0,
        Dynamic = 1
    };

    struct VertexBuffer
    {
#ifdef VN_RENDERER_API_OPENGL
        OpenGLVertexBuffer OpenGL;
#endif
        memorysize 	SizeInBytes;
        BufferUsage Usage;
    };

    struct IndexBuffer
    {
    #ifdef VN_RENDERER_API_OPENGL
        OpenGLIndexBuffer OpenGL;
    #endif

        memorysize  SizeInBytes;
        BufferUsage Usage;
    };
}