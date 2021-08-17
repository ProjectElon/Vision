#pragma once

#include "Vision/Core/Defines.h"

#include <glm/glm.hpp>

#ifdef VN_RENDERER_API_OPENGL
    #include <glad/glad.h>
#endif

namespace Vision
{
    enum WrapMode : uint8
    {
        WrapMode_Repeat = 0,
        WrapMode_ClampToEdge = 1,
    };

    enum FilterMode : uint8
    {
        FilterMode_Point = 0,
        FilterMode_Bilinear = 1
    };

    enum PixelFormat : uint8
    {
        PixelFormat_R    = 0,
        PixelFormat_RGB  = 1,
        PixelFormat_RGBA = 2
    };

    struct TextureRect
    {
        int32 X = 0;
        int32 Y = 0;

        uint32 Width  = 0;
        uint32 Height = 0;
    };

    struct UVRect
    {
        glm::vec2 BottomLeft;
        glm::vec2 TopRight;
    };

    struct Texture
    {
        union
        {
#ifdef VN_RENDERER_API_OPENGL
            struct
            {
                GLuint TextureID = 0;
            }
            OpenGL;
#endif
        };

        uint32 Width = 0;
        uint32 Height = 0;

        WrapMode WrapX = WrapMode_Repeat;
        WrapMode WrapY = WrapMode_Repeat;

        FilterMode Filter = FilterMode_Bilinear;
        PixelFormat Format = PixelFormat_RGBA;
    };

#ifdef VN_RENDERER_API_OPENGL

    void OpenGLInitTexture(Texture* texture,
                           void* pixels,
                           uint32 width,
                           uint32 height,
                           PixelFormat pixelFormat,
                           WrapMode wrapModeX,
                           WrapMode wrapModeY,
                           FilterMode filter);

    void OpenGLUninitTexture(Texture* texture);
    void OpenGLBindTexture(Texture* texture, uint32 textureSlot);
    void OpenGLUnbindTexture(Texture* texture);

    void OpenGLSetTextureWrapMode(Texture* texture,
                                  WrapMode wrapModeX,
                                  WrapMode wrapModeY);

    void OpenGLSetTextureFilterMode(Texture* texture,
                                    FilterMode filter);
#endif
}