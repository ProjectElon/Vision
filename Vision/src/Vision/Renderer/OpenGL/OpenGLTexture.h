#pragma once

#include "Vision/Renderer/RendererTypes.h"

namespace Vision
{
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
    
    void OpenGLSetTextureWrapMode(Texture* texture,
        WrapMode wrapModeX,
        WrapMode wrapModeY);

    void OpenGLSetTextureFilterMode(Texture* texture,
        FilterMode filter);
}