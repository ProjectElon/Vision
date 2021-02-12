#pragma once

#include "Vision/Core/Common.h"
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

    void InitTextureAtlasVaryingRects(TextureAtlas* atlas,
                                        AssetID textureID,
                                        uint32 rectCount,
                                        TextureRect* textureRects);

    void UninitTextureAtlas(TextureAtlas* atlas);

    void InitTextureAtlasGrid(TextureAtlasGrid* atlas,
                                AssetID textureID,
                                uint32 cellWidth,
                                uint32 cellHeight);

    void UninitTextureAtlasGird(TextureAtlasGrid* atlas);
}