#pragma once

#include "Vision/Renderer/RendererTypes.h"

namespace Vision
{
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
}