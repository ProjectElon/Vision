#pragma once

#include "Vision/Core/Base.h"
#include "Vision/Renderer/Texture2D.h"

#include <glm/glm.hpp>

namespace Vision
{
    /*@Robustness: Implement Texture Atlas with pixel perfect coords in the shader
                   in case if the floating point precision.
    */

    struct TextureRect
    {
        int32 X = 0;
        int32 Y = 0;

        uint32 Width = 0;
        uint32 Height = 0;
    };

    struct UVRect
    {
        glm::vec2 BottomLeft;
        glm::vec2 TopRight;
    };

    struct TextureAtlas
    {
        Texture2D* Texture = nullptr;

        uint32  RectCount = 0;
        UVRect* Rects     = nullptr;
    };

    struct TextureAtlasGrid : public TextureAtlas
    {
        uint32 Rows;
        uint32 Cols;
        uint32 CellWidth;
        uint32 CellHeight;
    };

    void CreateTextureAtlas(TextureAtlas* atlas, Texture2D* texture, uint32 rectCount, TextureRect* rects);
    void CreateTextureAtlasGrid(TextureAtlasGrid* atlas, Texture2D* texture, uint32 cellWidth, uint32 cellHeight);

    UVRect& GetUVRect(TextureAtlasGrid* atlas, uint32 row, uint32 col);

    void FreeTextureAtlas(TextureAtlas* atlas);

    void AllocateTextureAtlas(TextureAtlas* atlas, Texture2D* texture, uint32 rectCount);

    UVRect ConvertTextureRectToUVRect(const TextureRect& rect, uint32 textureWidth, uint32 textureHeight);
}