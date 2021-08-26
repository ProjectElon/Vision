#pragma once

#include "Vision/Core/Defines.h"

namespace Vision
{
    // @(Harlequin): Temprary
#define MaxColorAttachments 4
#define MaxShaderCount 4
#define MaxUniformCount 128

    struct OpenGLVertexBuffer
    {
        uint32 VertexBufferObject;
        uint32 VertexArrayObject;
    };

    struct OpenGLIndexBuffer
    {
        uint32 ElementBufferObject;
    };

    struct OpenGLTexture
    {
        uint32 TextureHandle;
    };

    struct OpenGLFrameBuffer
    {
        uint32 FrameBufferObject;
        uint32 ColorAttachments[4];
        uint32 DepthAttachment;
    };

    struct OpenGLUniform
    {
        const char* Name = "";
        int32 Location   = -1;
    };

    struct OpenGLShader
    {
        uint32 ProgramHandle;
        uint32 Shaders[MaxShaderCount];
        uint32 UniformCount;
        OpenGLUniform Uniforms[MaxUniformCount];
    };
}
