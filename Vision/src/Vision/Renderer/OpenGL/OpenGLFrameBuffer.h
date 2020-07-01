#pragma once

#include "Vision/Renderer/FrameBuffer.h"

namespace Vision
{
    class OpenGLFrameBuffer : public FrameBuffer
    {
    public:
        OpenGLFrameBuffer(const FrameBufferProps& frameBufferProps);
        ~OpenGLFrameBuffer();

        void Resize(uint32 width, uint32 height) override;

        void Bind() override;
        void UnBind() override;

        inline uint32 GetColorAttachmentRendererId() override
        {
            return m_ColorAttachmentRendererId;
        }

    private:
        FrameBufferProps m_Props;
        uint32 m_RendererId;
        uint32 m_ColorAttachmentRendererId;

        void Invalidate();
    };
}