#pragma once

#include "Vision/Core/Common.h"
#include "Vision/Renderer/Texture.h"

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

        RedInt32,
        RedUInt32
    };

    struct FrameBufferTextureSpecification
    {
        FrameBufferTextureFormat TextureFormat = FrameBufferTextureFormat::None;

        WrapMode WrapX = WrapMode::ClampToEdge;
        WrapMode WrapY = WrapMode::ClampToEdge;

        FilterMode FilterMode = FilterMode::Point;

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
        uint32 RendererID = 0;
        uint32 Width = 0;
        uint32 Height = 0;
        uint32 Samples = 1;

        std::vector<uint32> ColorAttachments;
        uint32 DepthAttachment = 0;

        std::vector<FrameBufferTextureSpecification> ColorAttachmentSpecification;
        FrameBufferTextureSpecification DepthAttachmentSpecification;

        bool SwapChainTarget = false;

        FrameBuffer() = default;

        FrameBuffer(const FrameBufferAttachmentSpecification& specification,
                    uint32 width,
                    uint32 height);

        ~FrameBuffer();

        void Init(const FrameBufferAttachmentSpecification& specification,
                  uint32 width,
                  uint32 height);

        void Uninit();

        void Resize(uint32 width, uint32 height);

        int32 ReadPixel(uint32 attachmentIndex, int32 x, int32 y);
        void ClearColorAttachment(uint32 index, const void* value);

        void Bind();
        void Unbind();
    };
}