#pragma once

#include "Vision/Core/Base.h"

namespace Vision
{
    struct FrameBuffer
    {
        uint32 Width; //@ReadOnly
        uint32 Height; //@ReadOnly
        uint32 RendererID; //@ReadOnly
        uint32 ColorAttachmentID; //@ReadOnly
    };

    void CreateFrameBuffer(FrameBuffer* frameBuffer, uint32 width, uint32 height);
    void DestroyFrameBuffer(FrameBuffer* frameBuffer);

    void ResizeFrameBuffer(FrameBuffer* frameBuffer, uint32 width, uint32 height);

    void BindFrameBuffer(FrameBuffer* frameBuffer);
    void UnBindFrameBuffer(FrameBuffer* frameBuffer);
}