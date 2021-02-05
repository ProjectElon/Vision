#include "pch.h"

#include "Vision/Core/Base.h"
#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

namespace Vision
{
     GLenum ShaderDataTypeToOpenGLType(ShaderDataType dataType)
     {
        switch (dataType)
        {
            case ShaderDataType::Bool:    return GL_BOOL;
            case ShaderDataType::Int8:    return GL_BYTE;
            case ShaderDataType::UInt8:   return GL_UNSIGNED_BYTE;
            case ShaderDataType::Int16:   return GL_SHORT;
            case ShaderDataType::UInt16:  return GL_UNSIGNED_SHORT;
            case ShaderDataType::Int32:   return GL_INT;
            case ShaderDataType::UInt32:  return GL_UNSIGNED_INT;
            case ShaderDataType::Float:   return GL_FLOAT;
            case ShaderDataType::Float2:  return GL_FLOAT;
            case ShaderDataType::Float3:  return GL_FLOAT;
            case ShaderDataType::Float4:  return GL_FLOAT;
            case ShaderDataType::Matrix3: return GL_FLOAT;
            case ShaderDataType::Matrix4: return GL_FLOAT;
        }

        VnCoreAssert(false, "unsupported shader data type");
     }

    uint32 GLTypeSize(ShaderDataType dataType)
    {
        switch (dataType)
        {
            case ShaderDataType::Bool:    return sizeof(GLboolean);
            case ShaderDataType::Int8:    return sizeof(GLbyte);
            case ShaderDataType::UInt8:   return sizeof(GLubyte);
            case ShaderDataType::Int16:   return sizeof(GLshort);
            case ShaderDataType::UInt16:  return sizeof(GLushort);
            case ShaderDataType::Int32:   return sizeof(GLint);
            case ShaderDataType::UInt32:  return sizeof(GLuint);
            case ShaderDataType::Float:   return 1 * sizeof(GLfloat);
            case ShaderDataType::Float2:  return 2 * sizeof(GLfloat);
            case ShaderDataType::Float3:  return 3 * sizeof(GLfloat);
            case ShaderDataType::Float4:  return 4 * sizeof(GLfloat);
            case ShaderDataType::Matrix3: return 3 * 3 * sizeof(GLfloat);
            case ShaderDataType::Matrix4: return 4 * 4 * sizeof(GLfloat);
        }

        VnCoreAssert(false, "unsupported shader data type");
    }

    uint32 GLTypeComponentCount(ShaderDataType dataType)
    {
        switch (dataType)
        {
            case ShaderDataType::Bool:    return 1;
            case ShaderDataType::Int8:    return 1;
            case ShaderDataType::UInt8:   return 1;
            case ShaderDataType::Int16:   return 1;
            case ShaderDataType::UInt16:  return 1;
            case ShaderDataType::Int32:   return 1;
            case ShaderDataType::UInt32:  return 1;
            case ShaderDataType::Float:   return 1;
            case ShaderDataType::Float2:  return 2;
            case ShaderDataType::Float3:  return 3;
            case ShaderDataType::Float4:  return 4;
            case ShaderDataType::Matrix3: return 3 * 3;
            case ShaderDataType::Matrix4: return 4 * 4;
        }

        VnCoreAssert(false, "unsupported shader data type");
    }

    GLenum GLInternalFormat(FrameBufferTextureFormat textureFormat)
    {
        switch (textureFormat)
        {
            case FrameBufferTextureFormat::RGBA8:     return GL_RGBA8;
            case FrameBufferTextureFormat::RedInt32:  return GL_R32I;
            case FrameBufferTextureFormat::RedUInt32: return GL_R32UI;
        }

        VnCoreAssert(false, "unsupported texture format");
    }

    GLenum GLTextureFormat(FrameBufferTextureFormat textureFormat)
    {
        switch (textureFormat)
        {
            case FrameBufferTextureFormat::RGBA8:     return GL_RGBA;
            case FrameBufferTextureFormat::RedInt32:  return GL_RED_INTEGER;
            case FrameBufferTextureFormat::RedUInt32: return GL_RED_INTEGER;
        }

        VnCoreAssert(false, "unsupported texture format");
    }

    GLenum GLInternalFormat(PixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
            case PixelFormat::R:    return GL_R8;
            case PixelFormat::RGB:  return GL_RGB8;
            case PixelFormat::RGBA: return GL_RGBA8;
            case PixelFormat::Font: return GL_RGBA8;
        }

        VnCoreAssert(false, "unsupported pixel format");
    }

    GLenum GLTextureFormat(PixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
            case PixelFormat::R:    return GL_RED;
            case PixelFormat::RGB:  return GL_RGB;
            case PixelFormat::RGBA: return GL_RGBA;
            case PixelFormat::Font: return GL_ALPHA;
        }

        VnCoreAssert(false, "unsupported pixel format");
    }


    GLenum GLWrapMode(WrapMode wrapMode)
    {
        switch(wrapMode)
        {
            case WrapMode::Repeat:      return GL_REPEAT;
            case WrapMode::ClampToEdge: return GL_CLAMP_TO_EDGE;
        }

        VnCoreAssert(false, "unsupported wrap mode");
    }

    GLenum GLFilterMode(FilterMode filterMode)
    {
        switch(filterMode)
        {
            case FilterMode::Point:    return GL_NEAREST;
            case FilterMode::Bilinear: return GL_LINEAR;
        }

        VnCoreAssert(false, "unsupported filter mode");
    }
}