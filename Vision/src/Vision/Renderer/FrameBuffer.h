#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Renderer/Texture.h"

#include <vector>
#include <initializer_list>

namespace Vision
{
    enum FrameBufferTextureFormat
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
            : Attachments(attachments) {}
    };

    #define MaxColorAttachments 4

    struct FrameBuffer
    {
        union
        {
#ifdef VN_RENDERER_API_OPENGL
            struct
            {
                uint32 FrameBufferObject;
                uint32 ColorAttachments[MaxColorAttachments];
                uint32 DepthAttachment;
            }
            OpenGL;
#endif
        };

        uint32 Width;
        uint32 Height;

        uint32 ColorAttachmentCount;
        FrameBufferTextureSpecification ColorAttachmentSpecification[MaxColorAttachments];
        FrameBufferTextureSpecification DepthAttachmentSpecification;
    };

#ifdef VN_RENDERER_API_OPENGL

    void OpenGLInitFrameBuffer(FrameBuffer* frameBuffer,
                               const FrameBufferAttachmentSpecification& specification,
                               uint32 width,
                               uint32 height);
    void OpenGLUninitFrameBuffer(FrameBuffer* frameBuffer);
    void OpenGLResizeFrameBuffer(FrameBuffer* frameBuffer,
                                 uint32 width,
                                 uint32 height);
    int32 OpenGLReadPixel(FrameBuffer* frameBuffer,
                          uint32 attachmentIndex,
                          int32 x,
                          int32 y);
    void OpenGLClearColorAttachment(FrameBuffer* frameBuffer,
                                    uint32 attachmentIndex,
                                    const void* value);
    void OpenGLBindFrameBuffer(FrameBuffer* frameBuffer);
    void OpenGLUnbindFrameBuffer(FrameBuffer* frameBuffer);

#endif
}