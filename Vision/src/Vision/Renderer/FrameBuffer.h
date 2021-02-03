#pragma once

#include "Vision/Core/Base.h"
#include <vector>
#include <initializer_list>

namespace Vision
{
    enum class FrameBufferTextureFormat
    {
        None = 0,

        RGBA8,

        Depth24Stencil8,

        Depth = Depth24Stencil8,

        RedInteger,
    };

    struct FrameBufferTextureSpecification
    {
        FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;
        // @(Harlequin): Filtering and Wraping

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

    struct FrameBuffer
    {
        uint32 Width = 0;
        uint32 Height = 0;
        uint32 Samples = 1;

        uint32 RendererID = 0;
        std::vector<uint32> ColorAttachments;
        uint32 DepthAttachment = 0;

        std::vector<FrameBufferTextureSpecification> ColorAttachmentSpecification;
        FrameBufferTextureSpecification DepthAttachmentFormat;

        bool SwapChainTarget = false;
    };

    void InitFrameBuffer(FrameBuffer* frameBuffer, FrameBufferAttachmentSpecification Attachments, uint32 width, uint32 height);
    void UninitFrameBuffer(FrameBuffer* frameBuffer);

    void ResizeFrameBuffer(FrameBuffer* frameBuffer, uint32 width, uint32 height);

    int ReadPixel(FrameBuffer* frameBuffer, uint32 attachmentIndex, int32 x, int32 y);

    void BindFrameBuffer(FrameBuffer* frameBuffer);
    void UnBindFrameBuffer(FrameBuffer* frameBuffer);
}