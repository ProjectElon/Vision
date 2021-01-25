#pragma once

#include "Vision/Core/Base.h"
#include "Vision/IO/Assets.h"
#include "Vision/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Vision
{
    /*
        @Robustness:
            Implement Texture Atlas with pixel perfect coords in the shader
            in case if the floating point precision.
    */
    struct TextureAtlas
    {
        AssetID TextureID = 0;

        uint32  RectCount = 0;
        UVRect *UVRects   = nullptr;
    };

    struct TextureAtlasGrid : public TextureAtlas
    {
        uint32 Rows = 0;
        uint32 Cols = 0;

        uint32 CellWidth  = 0;
        uint32 CellHeight = 0;
    };

    void CreateTextureAtlasVaryingRects(TextureAtlas* atlas,
                                        AssetID textureID,
                                        uint32 rectCount,
                                        TextureRect* textureRects);

    void DestroyTextureAtlas(TextureAtlas* atlas);

    void CreateTextureAtlasGrid(TextureAtlasGrid* atlas,
                                AssetID textureID,
                                uint32 cellWidth,
                                uint32 cellHeight);

    void DestroyTextureAtlasGird(TextureAtlasGrid* atlas);

    UVRect& GetUVRectFromTextureAtlasGird(TextureAtlasGrid* atlas,
                                          uint32 row,
                                          uint32 col);
}