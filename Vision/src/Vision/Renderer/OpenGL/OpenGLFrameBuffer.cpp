#include "pch.h"
#include <glad/glad.h>
#include "Vision/Renderer/FrameBuffer.h"

namespace Vision
{
    static void InvalidateFrameBuffer(FrameBuffer* frameBuffer)
    {
        if (frameBuffer->RendererID)
        {
            glDeleteFramebuffers(1, &frameBuffer->RendererID);
            glDeleteTextures(1, &frameBuffer->ColorAttachmentID);
        }

        glGenFramebuffers(1, &frameBuffer->RendererID);
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->RendererID);

        glGenTextures(1, &frameBuffer->ColorAttachmentID);
        glBindTexture(GL_TEXTURE_2D, frameBuffer->ColorAttachmentID);
        glTexImage2D(GL_TEXTURE_2D,
                     0,
                     GL_RGBA8,
                     frameBuffer->Width,
                     frameBuffer->Height,
                     0,
                     GL_RGBA,
                     GL_UNSIGNED_BYTE,
                     nullptr);

        glTextureParameteri(frameBuffer->ColorAttachmentID, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTextureParameteri(frameBuffer->ColorAttachmentID, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, frameBuffer->ColorAttachmentID, 0);

        VnCoreAssert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE, "Incomplete Framebuffer");

        glBindTexture(GL_TEXTURE_2D, 0);
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }

    void CreateFrameBuffer(FrameBuffer* frameBuffer, uint32 width, uint32 height)
    {
        frameBuffer->Width = width;
        frameBuffer->Height = height;
        frameBuffer->RendererID = 0;
        frameBuffer->ColorAttachmentID = 0;
        
        InvalidateFrameBuffer(frameBuffer);
    }

    void ResizeFrameBuffer(FrameBuffer* frameBuffer, uint32 width, uint32 height)
    {
        if (width == 0 || height == 0 || width < 100 || height < 100)
        {
            VnCoreWarn("invalid framebuffer size ({}, {})", width, height);
            return;
        }

        frameBuffer->Width = width;
        frameBuffer->Height = height;

        InvalidateFrameBuffer(frameBuffer);
    }

    void DestroyFrameBuffer(FrameBuffer* frameBuffer)
    {
        glDeleteFramebuffers(1, &frameBuffer->RendererID);
        glDeleteTextures(1, &frameBuffer->ColorAttachmentID);
        
        // @(Harlequin): Not sure 0 is valid or not in opengl land
        frameBuffer->RendererID = 0;
        frameBuffer->ColorAttachmentID = 0;
    }

    void BindFrameBuffer(FrameBuffer* frameBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, frameBuffer->RendererID);
        glViewport(0, 0, frameBuffer->Width, frameBuffer->Height);
    }

    void UnBindFrameBuffer(FrameBuffer* frameBuffer)
    {
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
}