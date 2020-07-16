#include "pch.h"
#include "Vision/Renderer/OpenGL/OpenGLFrameBuffer.h"
#include <glad/glad.h>

namespace Vision
{
    OpenGLFrameBuffer::OpenGLFrameBuffer(const FrameBufferProps& frameBufferProps)
        : m_Props(frameBufferProps)
        , m_RendererId(0)
        , m_ColorAttachmentRendererId(0)
    {
        Invalidate();
    }

    void OpenGLFrameBuffer::Resize(uint32 width, uint32 height)
    {
        if (width == 0 || height == 0 || width < 100 || height < 100)
        {
            VN_CORE_WARN("invalid framebuffer size ({}, {})", width, height);
            return;
        }

        m_Props.Width = width;
        m_Props.Height = height;

        Invalidate();
    }

    OpenGLFrameBuffer::~OpenGLFrameBuffer()
    {
        glDeleteFramebuffers(1, &m_RendererId);
        glDeleteTextures(1, &m_ColorAttachmentRendererId);
    }

    void OpenGLFrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);
        glViewport(0, 0, m_Props.Width, m_Props.Height);
    }

    void OpenGLFrameBuffer::UnBind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLFrameBuffer::Invalidate()
    {
        if (m_RendererId)
        {
            glDeleteFramebuffers(1, &m_RendererId);
            glDeleteTextures(1, &m_ColorAttachmentRendererId);
        }

        glGenFramebuffers(1, &m_RendererId);
        glBindFramebuffer(GL_FRAMEBUFFER, m_RendererId);

        glGenTextures(1, &m_ColorAttachmentRendererId);
        glBindTexture(GL_TEXTURE_2D, m_ColorAttachmentRendererId);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, m_Props.Width, m_Props.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        
        glTextureParameteri(m_ColorAttachmentRendererId, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(m_ColorAttachmentRendererId, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentRendererId, 0);

        VN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Incomplete Framebuffer");

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);   
    }
}