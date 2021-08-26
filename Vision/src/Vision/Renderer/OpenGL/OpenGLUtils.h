#pragma once

#include "Vision/Renderer/RendererTypes.h"

#include <glad/glad.h>

namespace Vision
{
    GLenum ShaderDataTypeToOpenGLType(ShaderDataType dataType);
    GLuint GLTypeSize(ShaderDataType dataType);
    GLuint GLTypeComponentCount(ShaderDataType dataType);

    GLenum GLInternalFormat(FrameBufferTextureFormat textureFormat);
    GLenum GLInternalFormat(PixelFormat pixelFormat);

    GLenum GLTextureFormat(FrameBufferTextureFormat textureFormat);
    GLenum GLTextureFormat(PixelFormat pixelFormat);

    GLenum GLWrapMode(WrapMode wrapMode);
    GLenum GLFilterMode(FilterMode filterMode);

    GLenum GLBufferUsage(BufferUsage usage);
}