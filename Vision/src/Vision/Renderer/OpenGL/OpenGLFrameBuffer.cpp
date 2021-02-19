#include "pch.h"
#include "Vision/Renderer/FrameBuffer.h"
#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

#include <glad/glad.h>

namespace Vision
{
    static void InvalidateFrameBuffer(FrameBuffer* frameBuffer)
    {
        if (frameBuffer->RendererID)
        {
            UninitFrameBuffer(frameBuffer);
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

        if (frameBuffer->DepthAttachmentFormat.TextureFormat != FrameBufferTextureFormat::None)
        {
            auto& depthSpec = frameBuffer->DepthAttachmentFormat;

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

    void InitFrameBuffer(FrameBuffer* frameBuffer, FrameBufferAttachmentSpecification Attachments, uint32 width, uint32 height)
    {
        frameBuffer->Width = width;
        frameBuffer->Height = height;
        frameBuffer->RendererID = 0;

        for (auto attachment : Attachments.Attachments)
        {
            if (attachment.TextureFormat == FrameBufferTextureFormat::Depth24Stencil8)
            {
                frameBuffer->DepthAttachmentFormat = FrameBufferTextureFormat::Depth24Stencil8;
            }
            else
            {
                frameBuffer->ColorAttachmentSpecification.push_back(attachment);
            }
        }

        InvalidateFrameBuffer(frameBuffer);
    }

    void ResizeFrameBuffer(FrameBuffer* frameBuffer, uint32 width, uint32 height)
    {
        if (width <= 0 || height <= 0)
        {
            VnCoreWarn("invalid framebuffer size ({}, {})", width, height);
            return;
        }

        frameBuffer->Width = width;
        frameBuffer->Height = height;

        InvalidateFrameBuffer(frameBuffer);
    }

    int32 ReadPixel(FrameBuffer* frameBuffer, uint32 attachmentIndex, int32 x, int32 y)
    {
        VnCoreAssert(attachmentIndex < frameBuffer->ColorAttachments.size(), "out of bounds");
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int pixel;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
        return pixel;
    }

    void UninitFrameBuffer(FrameBuffer* frameBuffer)
    {
        glDeleteFramebuffers(1, &frameBuffer->RendererID);

        if (!frameBuffer->ColorAttachments.empty())
        {
            glDeleteTextures(frameBuffer->ColorAttachments.size(), frameBuffer->ColorAttachments.data());
            memset(frameBuffer->ColorAttachments.data(), 0, sizeof(uint32) * frameBuffer->ColorAttachments.size());
        }

        if (frameBuffer->DepthAttachment)
        {
            glDeleteTextures(1, &frameBuffer->DepthAttachment);
            frameBuffer->DepthAttachment = 0;
        }
    }

    void BindFrameBuffer(FrameBuffer* frameBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->RendererID);
        glViewport(0, 0, frameBuffer->Width, frameBuffer->Height);
    }

    void ClearColorAttachment(FrameBuffer* frameBuffer, uint32 index, const void* value)
    {
        auto& spec = frameBuffer->ColorAttachmentSpecification[index];

        GLenum type = (spec.TextureFormat == FrameBufferTextureFormat::RedInt32) ? GL_INT : GL_FLOAT;

        GLenum textureFormat = GLTextureFormat(spec.TextureFormat);

        glClearTexImage(frameBuffer->ColorAttachments[index],
                        0,
                        textureFormat,
                        type,
                        value);
    }

    void UnBindFrameBuffer(FrameBuffer* frameBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}