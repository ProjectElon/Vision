#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"

#ifdef VN_RENDERER_API_OPENGL

#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/OpenGL/OpenGLFrameBuffer.h"
#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

#include <glad/glad.h>

namespace Vision
{
    static void InvalidateFrameBuffer(FrameBuffer* frameBuffer)
    {
        OpenGLUninitFrameBuffer(frameBuffer);

        auto& openglFrameBuffer = frameBuffer->OpenGL;

        glGenFramebuffers(1, &openglFrameBuffer.FrameBufferObject);
        glBindFramebuffer(GL_FRAMEBUFFER, openglFrameBuffer.FrameBufferObject);

        for (uint32 attachmentIndex = 0;
             attachmentIndex < frameBuffer->ColorAttachmentCount;
             attachmentIndex++)
        {
            auto& colorSpec = frameBuffer->ColorAttachmentSpecification[attachmentIndex];

            glCreateTextures(GL_TEXTURE_2D, 1, &openglFrameBuffer.ColorAttachments[attachmentIndex]);
            glBindTexture(GL_TEXTURE_2D, openglFrameBuffer.ColorAttachments[attachmentIndex]);

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

            glTextureParameteri(openglFrameBuffer.ColorAttachments[attachmentIndex],
                                GL_TEXTURE_MIN_FILTER,
                                GLFilterMode(colorSpec.FilterMode));

            glTextureParameteri(openglFrameBuffer.ColorAttachments[attachmentIndex],
                                GL_TEXTURE_MAG_FILTER,
                                GLFilterMode(colorSpec.FilterMode));

            glTextureParameteri(openglFrameBuffer.ColorAttachments[attachmentIndex],
                                GL_TEXTURE_WRAP_S,
                                GLWrapMode(colorSpec.WrapX));

            glTextureParameteri(openglFrameBuffer.ColorAttachments[attachmentIndex],
                                GL_TEXTURE_WRAP_T,
                                GLWrapMode(colorSpec.WrapY));

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_COLOR_ATTACHMENT0 + attachmentIndex,
                                   GL_TEXTURE_2D,
                                   openglFrameBuffer.ColorAttachments[attachmentIndex],
                                   0);
        }

        if (frameBuffer->ColorAttachmentCount > 1)
        {
            GLenum buffers[4] =
            {
                GL_COLOR_ATTACHMENT0,
                GL_COLOR_ATTACHMENT1,
                GL_COLOR_ATTACHMENT2,
                GL_COLOR_ATTACHMENT3
            };

            VnCoreAssert(frameBuffer->ColorAttachmentCount <= 4,
                         "max color attachments allowed is 4");
            glDrawBuffers(frameBuffer->ColorAttachmentCount, buffers);
        }
        else
        {
            glDrawBuffer(GL_NONE);
        }

        if (frameBuffer->DepthAttachmentSpecification.TextureFormat != FrameBufferTextureFormat_None)
        {
            auto& depthSpec = frameBuffer->DepthAttachmentSpecification;

            glCreateTextures(GL_TEXTURE_2D, 1, &openglFrameBuffer.DepthAttachment);
            glBindTexture(GL_TEXTURE_2D, openglFrameBuffer.DepthAttachment);
            glTexStorage2D(GL_TEXTURE_2D,
                           1,
                           GL_DEPTH24_STENCIL8,
                           frameBuffer->Width,
                           frameBuffer->Height);

            glTextureParameteri(openglFrameBuffer.DepthAttachment,
                                GL_TEXTURE_MIN_FILTER,
                                GLFilterMode(depthSpec.FilterMode));

            glTextureParameteri(openglFrameBuffer.DepthAttachment,
                                GL_TEXTURE_MAG_FILTER,
                                GLFilterMode(depthSpec.FilterMode));

            glTextureParameteri(openglFrameBuffer.DepthAttachment,
                                GL_TEXTURE_WRAP_S,
                                GLWrapMode(depthSpec.WrapX));

            glTextureParameteri(openglFrameBuffer.DepthAttachment,
                                GL_TEXTURE_WRAP_T,
                                GLWrapMode(depthSpec.WrapY));

            glFramebufferTexture2D(GL_FRAMEBUFFER,
                                   GL_DEPTH_STENCIL_ATTACHMENT,
                                   GL_TEXTURE_2D,
                                   openglFrameBuffer.DepthAttachment,
                                   0);
        }

        VnCoreAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE,
                     "incomplete framebuffer");
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void OpenGLInitFrameBuffer(FrameBuffer* frameBuffer,
                               const FrameBufferAttachmentSpecification& specification,
                               uint32 width,
                               uint32 height)
    {
        auto& openglFrameBuffer = frameBuffer->OpenGL;

        frameBuffer->Width  = width;
        frameBuffer->Height = height;
        frameBuffer->ColorAttachmentCount = 0;
        frameBuffer->DepthAttachmentSpecification = FrameBufferTextureFormat_None;

        openglFrameBuffer.FrameBufferObject = 0;
        openglFrameBuffer.DepthAttachment = 0;

        auto& spec = specification.Attachments;

        for (uint32 attachmentIndex = 0;
             attachmentIndex < spec.size();
             attachmentIndex++)
        {
            if (spec[attachmentIndex].TextureFormat == FrameBufferTextureFormat_Depth24Stencil8)
            {
                frameBuffer->DepthAttachmentSpecification = spec[attachmentIndex].TextureFormat;
            }
            else
            {
                uint32 colorAttachmentIndex = frameBuffer->ColorAttachmentCount;
                frameBuffer->ColorAttachmentSpecification[colorAttachmentIndex] = spec[attachmentIndex];
                frameBuffer->ColorAttachmentCount++;
            }
        }

        InvalidateFrameBuffer(frameBuffer);
    }

    void OpenGLUninitFrameBuffer(FrameBuffer* frameBuffer)
    {
        auto& openglFrameBuffer = frameBuffer->OpenGL;

        if (openglFrameBuffer.FrameBufferObject)
        {
            glDeleteFramebuffers(1, &openglFrameBuffer.FrameBufferObject);
            openglFrameBuffer.FrameBufferObject = 0;
        }

        if (frameBuffer->ColorAttachmentCount)
        {
            glDeleteTextures(frameBuffer->ColorAttachmentCount,
                             openglFrameBuffer.ColorAttachments);
            memset(openglFrameBuffer.ColorAttachments,
                   0,
                   sizeof(int32) * frameBuffer->ColorAttachmentCount);
        }

        if (frameBuffer->OpenGL.DepthAttachment)
        {
            glDeleteTextures(1, &openglFrameBuffer.DepthAttachment);
            openglFrameBuffer.DepthAttachment = 0;
        }
    }

    void OpenGLResizeFrameBuffer(FrameBuffer* frameBuffer,
                                 uint32 width,
                                 uint32 height)
    {
        if (width <= 0 || height <= 0)
        {
            VnCoreWarn("invalid framebuffer size ({0}, {1})", width, height);
            return;
        }

        frameBuffer->Width  = width;
        frameBuffer->Height = height;

        InvalidateFrameBuffer(frameBuffer);
    }

    int32 OpenGLReadPixel(FrameBuffer* frameBuffer,
                          uint32 attachmentIndex,
                          int32 x,
                          int32 y)
    {
        VnCoreAssert(attachmentIndex < frameBuffer->ColorAttachmentCount, "out of bounds");
        glReadBuffer(GL_COLOR_ATTACHMENT0 + attachmentIndex);
        int32 pixel;
        glReadPixels(x, y, 1, 1, GL_RED_INTEGER, GL_INT, &pixel);
        return pixel;
    }

    void OpenGLClearColorAttachment(FrameBuffer* frameBuffer,
                                    uint32 attachmentIndex,
                                    const void* value)
    {
        auto& openglFrameBuffer = frameBuffer->OpenGL;

        auto& spec = frameBuffer->ColorAttachmentSpecification[attachmentIndex];

        GLenum type = (spec.TextureFormat == FrameBufferTextureFormat_RedInt32) ? GL_INT : GL_FLOAT;

        GLenum textureFormat = GLTextureFormat(spec.TextureFormat);

        glClearTexImage(openglFrameBuffer.ColorAttachments[attachmentIndex],
                        0,
                        textureFormat,
                        type,
                        value);
    }

    void OpenGLBindFrameBuffer(FrameBuffer* frameBuffer)
    {
        auto& openglFrameBuffer = frameBuffer->OpenGL;
        glBindFramebuffer(GL_FRAMEBUFFER, openglFrameBuffer.FrameBufferObject);
        ViewportRect viewportRect = { 0, 0, frameBuffer->Width, frameBuffer->Height };
        Renderer::API.SetViewport(&viewportRect);
    }

    void OpenGLUnbindFrameBuffer(FrameBuffer* frameBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}

#endif