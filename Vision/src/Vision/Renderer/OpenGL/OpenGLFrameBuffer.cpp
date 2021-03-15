#include "pch.h"
#include "Vision/Renderer/FrameBuffer.h"
#include "Vision/Renderer/OpenGL/OpenGLUtils.h"
#include "Vision/Renderer/Renderer.h"

#include <glad/glad.h>

namespace Vision
{
    static void InvalidateFrameBuffer(FrameBuffer* frameBuffer)
    {
        if (frameBuffer->RendererID)
        {
            frameBuffer->Uninit();
        }

        glGenFramebuffers(1, &frameBuffer->RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->RendererID);

        if (frameBuffer->ColorAttachments.empty())
        {
            frameBuffer->ColorAttachments.resize(frameBuffer->ColorAttachmentSpecification.size());
        }

        for (uint32 attachmentIndex = 0;
             attachmentIndex < frameBuffer->ColorAttachmentSpecification.size();
             attachmentIndex++)
        {
            auto& colorSpec = frameBuffer->ColorAttachmentSpecification[attachmentIndex];

            glCreateTextures(GL_TEXTURE_2D, 1, &frameBuffer->ColorAttachments[attachmentIndex]);
            glBindTexture(GL_TEXTURE_2D, frameBuffer->ColorAttachments[attachmentIndex]);

            GLenum internalFormat = GLInternalFormat(colorSpec.TextureFormat);
            GLenum textureFormat = GLTextureFormat(colorSpec.TextureFormat);

            glTexImage2D(GL_TEXTURE_2D,
                         0,
                         internalFormat,
                         frameBuffer->Width,
                         frameBuffer->Height,
                         0,
                         textureFormat,
                         GL_UNSIGNED_BYTE,
                         nullptr);

            glTextureParameteri(frameBuffer->ColorAttachments[attachmentIndex], GL_TEXTURE_MIN_FILTER, GLFilterMode(colorSpec.FilterMode));
            glTextureParameteri(frameBuffer->ColorAttachments[attachmentIndex], GL_TEXTURE_MAG_FILTER, GLFilterMode(colorSpec.FilterMode));

            glTextureParameteri(frameBuffer->ColorAttachments[attachmentIndex], GL_TEXTURE_WRAP_S, GLWrapMode(colorSpec.WrapX));
            glTextureParameteri(frameBuffer->ColorAttachments[attachmentIndex], GL_TEXTURE_WRAP_T, GLWrapMode(colorSpec.WrapY));

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attachmentIndex,
                                   GL_TEXTURE_2D,
                                   frameBuffer->ColorAttachments[attachmentIndex],
                                   0);
        }

        if (frameBuffer->ColorAttachmentSpecification.size() > 1)
        {
            GLenum buffers[4] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3
            };

            VnCoreAssert(frameBuffer->ColorAttachments.size() <= 4, "max color attachments allowed is 4");
            glDrawBuffers(frameBuffer->ColorAttachments.size(), buffers);
        }
        else
        {
            glDrawBuffer(GL_NONE);
        }

        if (frameBuffer->DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat::None)
        {
            auto& depthSpec = frameBuffer->DepthAttachmentSpecification;

            glCreateTextures(GL_TEXTURE_2D, 1, &frameBuffer->DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, frameBuffer->DepthAttachment);
            glTexStorage2D(GL_TEXTURE_2D,
                           1,
                           GL_DEPTH24_STENCIL8,
                           frameBuffer->Width,
                           frameBuffer->Height);

            glTextureParameteri(frameBuffer->DepthAttachment, GL_TEXTURE_MIN_FILTER, GLFilterMode(depthSpec.FilterMode));
            glTextureParameteri(frameBuffer->DepthAttachment, GL_TEXTURE_MAG_FILTER, GLFilterMode(depthSpec.FilterMode));

            glTextureParameteri(frameBuffer->DepthAttachment, GL_TEXTURE_WRAP_S, GLWrapMode(depthSpec.WrapX));
            glTextureParameteri(frameBuffer->DepthAttachment, GL_TEXTURE_WRAP_T, GLWrapMode(depthSpec.WrapY));

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_TEXTURE_2D,
                                   frameBuffer->DepthAttachment,
                                   0);
        }

        VnCoreAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Incomplete Framebuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }


    FrameBuffer::FrameBuffer(const FrameBufferAttachmentSpecification& specification,
                             uint32 width,
                             uint32 height)
    {
        Init(specification, width, height);
    }

    FrameBuffer::~FrameBuffer()
    {
        Uninit();
    }

    void FrameBuffer::Init(const FrameBufferAttachmentSpecification& specification,
                           uint32 width,
                           uint32 height)
    {
        Width  = width;
        Height = height;
        RendererID = 0;

        for (const auto& attachment : specification.Attachments)
        {
            if (attachment.TextureFormat == FrameBufferTextureFormat::Depth24Stencil8)
            {
                DepthAttachmentSpecification = FrameBufferTextureFormat::Depth24Stencil8;
            }
            else
            {
                ColorAttachmentSpecification.push_back(attachment);
            }
        }

        InvalidateFrameBuffer(this);
    }

    void FrameBuffer::Uninit()
    {
        glDeleteFramebuffers(1, &RendererID);

        if (!ColorAttachments.empty())
        {
            glDeleteTextures(ColorAttachments.size(), ColorAttachments.data());
            memset(ColorAttachments.data(), 0, sizeof(uint32) * ColorAttachments.size());
        }

        if (DepthAttachment)
        {
            glDeleteTextures(1, &DepthAttachment);
            DepthAttachment = 0;
        }
    }

    void FrameBuffer::Resize(uint32 width, uint32 height)
    {
        if (width <= 0 || height <= 0)
        {
            VnCoreWarn("invalid framebuffer size ({0}, {1})", width, height);
            return;
        }

        Width = width;
        Height = height;

        InvalidateFrameBuffer(this);
    }

    int32 FrameBuffer::ReadPixel(uint32 attachmentIndex, int32 x, int32 y)
    {
        VnCoreAssert(attachmentIndex < ColorAttachments.size(), "out of bounds");
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int32 pixel;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
        return pixel;
    }

    void FrameBuffer::ClearColorAttachment(uint32 index, const void* value)
    {
        auto& spec = ColorAttachmentSpecification[index];

        GLenum type = (spec.TextureFormat == FrameBufferTextureFormat::RedInt32) ? GL_INT : GL_FLOAT;

        GLenum textureFormat = GLTextureFormat(spec.TextureFormat);

        glClearTexImage(ColorAttachments[index],
                        0,
                        textureFormat,
                        type,
                        value);
    }

    void FrameBuffer::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, RendererID);
        Renderer::SetViewport(0, 0, Width, Height);
    }

    void FrameBuffer::Unbind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}