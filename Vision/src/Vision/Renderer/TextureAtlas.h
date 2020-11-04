#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Renderer/Texture2D.h"

#include <glm/glm.hpp>

namespace Vision
{
    /*@Robustness: Implement Texture Atlas with pixel perfect coords in the shader
                   in case if the floating point precision.
    */

    struct UVRect
    {
        glm::vec2 BottomLeft;
        glm::vec2 TopRight;
    };

    struct TextureRect
    {
        int32 X = 0;
        int32 Y = 0;

        uint32 Width = 0;
        uint32 Height = 0;
    };

    struct TextureAtlas
    {
        Texture2D* Texture = nullptr;

        uint32  RectCount = 0;
        UVRect* Rects     = nullptr;
    };

    TextureAtlas CreateTextureAtlas(Texture2D* texture, uint32 rectCount, TextureRect* rects);
    TextureAtlas CreateTextureAtlas(Texture2D* texture, uint32 cellCount, uint32 cellWidth, uint32 cellHeight);

    void FreeTextureAtlas(TextureAtlas* atlas);
}