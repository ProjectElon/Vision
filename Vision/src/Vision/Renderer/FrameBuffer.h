#pragma once

#include "Vision/Core/Core.h"

namespace Vision
{
    struct FrameBufferProps
    {
        uint32 Width;
        uint32 Height;
    };

    class FrameBuffer
    {
    public:
        FrameBuffer() = default;

        virtual ~FrameBuffer() {}

        virtual void Resize(uint32 width, uint32 height) = 0;

        virtual void Bind() = 0;
        virtual void UnBind() = 0;

        virtual uint32 GetColorAttachmentRendererId() = 0;
        
        static Ref<FrameBuffer> Create(const FrameBufferProps& frameBufferPros);
    };
}