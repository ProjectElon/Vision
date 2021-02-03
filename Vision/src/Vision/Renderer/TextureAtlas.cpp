#include "pch.h"

#include "Vision/Renderer/TextureAtlas.h"

namespace Vision
{
    static void AllocateTextureAtlas(TextureAtlas* atlas,
                                     AssetID textureID,
                                     uint32 rectCount)
    {
        VnCoreAssert(atlas->UVRects == nullptr && rectCount > 0, "invalid texture atlas allocation");

        atlas->TextureID = textureID;
        atlas->RectCount = rectCount;

        atlas->UVRects = new UVRect[rectCount];
    }

    static void ConvertTextureRectToUVRect(const TextureRect* textureRect,
                                           uint32 textureWidth,
                                           uint32 textureHeight,
                                           UVRect* uvRect)
    {
        using namespace glm;

        vec2 topLeft       = { textureRect->X, textureHeight - textureRect->Y };
        vec2 bottomRight   = { textureRect->X + textureRect->Width, textureHeight - (textureRect->Y + textureRect->Height) };

        vec2 topLeftUV     = { topLeft.x / textureWidth, topLeft.y / textureHeight };
        vec2 bottomRightUV = { bottomRight.x / textureWidth, bottomRight.y / textureHeight };

        uvRect->BottomLeft = { topLeftUV.x, bottomRightUV.y };
        uvRect->TopRight   = { bottomRightUV.x, topLeftUV.y };
    }

    void InitTextureAtlasVaryingRects(TextureAtlas* atlas,
                                        AssetID textureID,
                                        uint32 rectCount,
                                        TextureRect* textureRects)
    {
        AllocateTextureAtlas(atlas,
                             textureID,
                             rectCount);

        Texture* texture = Assets::GetTexture(textureID);

        for (uint32 rectIndex = 0;
             rectIndex < rectCount;
             ++rectIndex)
        {
            ConvertTextureRectToUVRect(&textureRects[rectIndex],
                                       texture->Width,
                                       texture->Height,
                                       &atlas->UVRects[rectIndex]);
        }
    }

    void InitTextureAtlasGrid(TextureAtlasGrid* atlas,
                                AssetID textureID,
                                uint32 cellWidth,
                                uint32 cellHeight)
    {
        Texture* texture = Assets::GetTexture(textureID);

        atlas->CellWidth = cellWidth;
        atlas->CellHeight = cellHeight;
        atlas->Rows = texture->Height / atlas->CellHeight;
        atlas->Cols = texture->Width / atlas->CellWidth;

        uint32 rectCount = atlas->Rows * atlas->Cols;

        AllocateTextureAtlas(atlas,
                             textureID,
                             rectCount);

        for (uint32 row = 0;
             row < atlas->Rows;
             ++row)
        {
            for (uint32 col = 0;
                 col < atlas->Cols;
                 ++col)
            {
                TextureRect currentTextureRect =
                {
                    col * cellWidth,
                    row * cellHeight,
                    cellWidth,
                    cellHeight
                };

                uint32 cuurentTextureRectIndex = row * atlas->Cols + col;
                ConvertTextureRectToUVRect(&currentTextureRect,
                                           texture->Width,
                                           texture->Height,
                                           &atlas->UVRects[cuurentTextureRectIndex]);
            }
        }
    }

    UVRect& GetUVRect(TextureAtlasGrid* atlas,
                      uint32 row,
                      uint32 col)
    {
        VnCoreAssert(row >= 0 && row < atlas->Rows && col >= 0 && col > atlas->Cols, "out of bounds");
        uint32 rectIndex = row * atlas->Cols  + col;
        return atlas->UVRects[rectIndex];
    }

    void UninitTextureAtlas(TextureAtlas* atlas)
    {
        VnCoreAssert(atlas->UVRects != nullptr, "can't free a nullptr");

        delete[] atlas->UVRects;

        atlas->UVRects     = nullptr;
        atlas->RectCount = 0;
    }

    void UninitTextureAtlasGird(TextureAtlasGrid* atlas)
    {
        UninitTextureAtlas((TextureAtlas*)atlas);
    }
}