#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"

#include "Vision/IO/Assets.h"
#include "Vision/Renderer/Texture.h"

#include <glm/glm.hpp>

namespace Vision
{
    /*
        @Robustness: Implement Texture Atlas with pixel perfect coords in the shader
                     in case if the floating point precision.
    */

    struct TextureAtlas
    {
        AssetID TextureID = 0;
        uint32  RectCount = 0;
        UVRect* UVRects   = nullptr;
    };

    struct TextureAtlasRects
    {
        TextureAtlas Atlas;

        TextureAtlasRects() = default;
        ~TextureAtlasRects();

        TextureAtlasRects(AssetID textureID,
                          uint32 rectCount,
                          TextureRect* textureRects);

        void Init(AssetID textureID,
                  uint32 rectCount,
                  TextureRect* textureRects);

        void Uninit();
    };

    struct TextureAtlasGrid
    {
        TextureAtlas Atlas;

        uint32 CellWidth  = 0;
        uint32 CellHeight = 0;

        uint32 Rows = 0;
        uint32 Cols = 0;

        TextureAtlasGrid() = default;
        ~TextureAtlasGrid();

        TextureAtlasGrid(AssetID textureID,
                         uint32 cellWidth,
                         uint32 cellHeight);

        void Init(AssetID textureID,
                  uint32 cellWidth,
                  uint32 cellHeight);

        void Uninit();

        inline const UVRect* GetUVRect(uint32 row, uint32 col)
        {
            VnCoreAssert(row >= 0 && row < Rows && col >= 0 && col > Cols);
            uint32 rectIndex = row * Cols  + col;
            return &Atlas.UVRects[rectIndex];
        }
    };
}