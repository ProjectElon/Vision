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
        m_Props.Width = width;
        m_Props.Height = height;

        Invalidate();

        VN_CORE_INFO("Creating framebuffer with size : ({0}, {1})", width, height);
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

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_Props.Width, m_Props.Height, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_ColorAttachmentRendererId, 0);

        VN_CORE_ASSERT(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "InComplete Framebuffer");

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);   
    }
}