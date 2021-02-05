#pragma once

#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/FrameBuffer.h"
#include "Vision/Renderer/Texture.h"

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
}