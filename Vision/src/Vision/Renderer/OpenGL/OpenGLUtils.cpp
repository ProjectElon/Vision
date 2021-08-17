#include "pch.hpp"

#include "Vision/Core/Defines.h"
#include "Vision/Core/Log.h"

#include "Vision/Renderer/OpenGL/OpenGLUtils.h"

namespace Vision
{
    GLenum GLBufferUsage(BufferUsage usage)
    {
        switch (usage)
        {
            case BufferUsage::Static:  return GL_STATIC_DRAW;
            case BufferUsage::Dynamic: return GL_DYNAMIC_DRAW;
        }

        VnCoreAssert(false, "unsupported buffser usage");
        return 0;
    }

     GLenum ShaderDataTypeToOpenGLType(ShaderDataType dataType)
     {
        switch (dataType)
        {
            case ShaderDataType_Bool:    return GL_BOOL;
            case ShaderDataType_Int8:    return GL_BYTE;
            case ShaderDataType_UInt8:   return GL_UNSIGNED_BYTE;
            case ShaderDataType_Int16:   return GL_SHORT;
            case ShaderDataType_UInt16:  return GL_UNSIGNED_SHORT;
            case ShaderDataType_Int32:   return GL_INT;
            case ShaderDataType_UInt32:  return GL_UNSIGNED_INT;
            case ShaderDataType_Float:   return GL_FLOAT;
            case ShaderDataType_Float2:  return GL_FLOAT;
            case ShaderDataType_Float3:  return GL_FLOAT;
            case ShaderDataType_Float4:  return GL_FLOAT;
            case ShaderDataType_Matrix3: return GL_FLOAT;
            case ShaderDataType_Matrix4: return GL_FLOAT;
        }

        VnCoreAssert(false, "unsupported shader data type");
        return GL_NONE;
     }

    uint32 GLTypeSize(ShaderDataType dataType)
    {
        switch (dataType)
        {
            case ShaderDataType_Bool:    return sizeof(GLboolean);
            case ShaderDataType_Int8:    return sizeof(GLbyte);
            case ShaderDataType_UInt8:   return sizeof(GLubyte);
            case ShaderDataType_Int16:   return sizeof(GLshort);
            case ShaderDataType_UInt16:  return sizeof(GLushort);
            case ShaderDataType_Int32:   return sizeof(GLint);
            case ShaderDataType_UInt32:  return sizeof(GLuint);
            case ShaderDataType_Float:   return 1 * sizeof(GLfloat);
            case ShaderDataType_Float2:  return 2 * sizeof(GLfloat);
            case ShaderDataType_Float3:  return 3 * sizeof(GLfloat);
            case ShaderDataType_Float4:  return 4 * sizeof(GLfloat);
            case ShaderDataType_Matrix3: return 3 * 3 * sizeof(GLfloat);
            case ShaderDataType_Matrix4: return 4 * 4 * sizeof(GLfloat);
        }

        VnCoreAssert(false, "unsupported shader data type");
        return 0;
    }

    uint32 GLTypeComponentCount(ShaderDataType dataType)
    {
        switch (dataType)
        {
            case ShaderDataType_Bool:    return 1;
            case ShaderDataType_Int8:    return 1;
            case ShaderDataType_UInt8:   return 1;
            case ShaderDataType_Int16:   return 1;
            case ShaderDataType_UInt16:  return 1;
            case ShaderDataType_Int32:   return 1;
            case ShaderDataType_UInt32:  return 1;
            case ShaderDataType_Float:   return 1;
            case ShaderDataType_Float2:  return 2;
            case ShaderDataType_Float3:  return 3;
            case ShaderDataType_Float4:  return 4;
            case ShaderDataType_Matrix3: return 3 * 3;
            case ShaderDataType_Matrix4: return 4 * 4;
        }

        VnCoreAssert(false, "unsupported shader data type");
        return 0;
    }

    GLenum GLInternalFormat(FrameBufferTextureFormat textureFormat)
    {
        switch (textureFormat)
        {
            case FrameBufferTextureFormat_RGBA8:     return GL_RGBA8;
            case FrameBufferTextureFormat_RedInt32:  return GL_R32I;
            case FrameBufferTextureFormat_RedUInt32: return GL_R32UI;
        }

        VnCoreAssert(false, "unsupported texture format");
        return 0;
    }

    GLenum GLTextureFormat(FrameBufferTextureFormat textureFormat)
    {
        switch (textureFormat)
        {
            case FrameBufferTextureFormat_RGBA8:     return GL_RGBA;
            case FrameBufferTextureFormat_RedInt32:  return GL_RED_INTEGER;
            case FrameBufferTextureFormat_RedUInt32: return GL_RED_INTEGER;
        }

        VnCoreAssert(false, "unsupported texture format");
        return 0;
    }

    GLenum GLInternalFormat(PixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
            case PixelFormat_R:    return GL_R8;
            case PixelFormat_RGB:  return GL_RGB8;
            case PixelFormat_RGBA: return GL_RGBA8;
        }

        VnCoreAssert(false, "unsupported pixel format");
        return 0;
    }

    GLenum GLTextureFormat(PixelFormat pixelFormat)
    {
        switch (pixelFormat)
        {
            case PixelFormat_R:    return GL_RED;
            case PixelFormat_RGB:  return GL_RGB;
            case PixelFormat_RGBA: return GL_RGBA;
        }

        VnCoreAssert(false, "unsupported pixel format");
        return 0;
    }


    GLenum GLWrapMode(WrapMode wrapMode)
    {
        switch(wrapMode)
        {
            case WrapMode_Repeat:      return GL_REPEAT;
            case WrapMode_ClampToEdge: return GL_CLAMP_TO_EDGE;
        }

        VnCoreAssert(false, "unsupported wrap mode");
        return 0;
    }

    GLenum GLFilterMode(FilterMode filterMode)
    {
        switch (filterMode)
        {
            case FilterMode_Point:    return GL_NEAREST;
            case FilterMode_Bilinear: return GL_LINEAR;
        }

        VnCoreAssert(false, "unsupported filter mode");
        return 0;
    }
}