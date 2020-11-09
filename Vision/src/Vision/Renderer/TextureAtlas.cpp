#include "pch.h"

#include "Vision/Renderer/TextureAtlas.h"

namespace Vision
{
    void CreateTextureAtlasVaryingRects(TextureAtlas* atlas,
                                        Texture2D* texture,
                                        uint32 rectCount,
                                        TextureRect* rects)
    {
        AllocateTextureAtlas(atlas, texture, rectCount);

        const TextureData& data = texture->GetData();

        uint32 textureWidth  = data.Width;
        uint32 textureHeight = data.Height;

        for (uint32 rectIndex = 0;
             rectIndex < rectCount;
             ++rectIndex)
        {
            atlas->Rects[rectIndex] = ConvertTextureRectToUVRect(rects[rectIndex], textureWidth, textureHeight);
        }
    }

    void CreateTextureAtlasGrid(TextureAtlasGrid* atlas,
                                Texture2D* texture,
                                uint32 cellWidth,
                                uint32 cellHeight)
    {
        const TextureData& textureData = texture->GetData();

        uint32 textureWidth  = textureData.Width;
        uint32 textureHeight = textureData.Height;

        atlas->CellWidth = cellWidth;
        atlas->CellHeight = cellHeight;
        atlas->Rows = textureHeight / atlas->CellHeight;
        atlas->Cols = textureWidth / atlas->CellWidth;

        uint32 rectCount = atlas->Rows * atlas->Cols;

        AllocateTextureAtlas(atlas, texture, rectCount);

        for (uint32 row = 0;
             row < atlas->Rows;
             ++row)
        {
            for (uint32 col = 0;
                 col < atlas->Cols;
                 ++col)
            {
                TextureRect currentRect = {
                    col * cellWidth,
                    row * cellHeight,
                    cellWidth,
                    cellHeight
                };
                uint32 rectIndex = row * atlas->Cols + col;
                atlas->Rects[rectIndex] = ConvertTextureRectToUVRect(currentRect, textureWidth, textureHeight);
            }
        }
    }

    UVRect& GetUVRect(TextureAtlasGrid* atlas,
                      uint32 row,
                      uint32 col)
    {
        VN_CORE_ASSERT(row >= 0 && row < atlas->Rows && col >= 0 && col > atlas->Cols, "out of bounds");

        uint32 rectIndex = row * atlas->Cols  + col;
        return atlas->Rects[rectIndex];
    }

    void FreeTextureAtlas(TextureAtlas* atlas)
    {
        VN_CORE_ASSERT(atlas->Rects != nullptr, "can't free a nullptr");

        delete[] atlas->Rects;

        atlas->Texture   = nullptr;
        atlas->Rects     = nullptr;
        atlas->RectCount = 0;
    }

    static void AllocateTextureAtlas(TextureAtlas* atlas, Texture2D* texture, uint32 rectCount)
    {
        VN_CORE_ASSERT(atlas->Rects == nullptr && texture != nullptr && rectCount > 0, "invalid texture atlas allocation");

        atlas->Texture   = texture;
        atlas->RectCount = rectCount;
        atlas->Rects     = new UVRect[rectCount];
    }

    static UVRect ConvertTextureRectToUVRect(const TextureRect& rect, uint32 textureWidth, uint32 textureHeight)
    {
        using namespace glm;

        vec2 topLeft       = { rect.X, textureHeight - rect.Y };
        vec2 bottomRight   = { rect.X + rect.Width, textureHeight - (rect.Y + rect.Height) };

        vec2 topLeftUV     = { topLeft.x / textureWidth, topLeft.y / textureHeight };
        vec2 bottomRightUV = { bottomRight.x / textureWidth, bottomRight.y / textureHeight };

        UVRect uv;
        uv.BottomLeft = { topLeftUV.x, bottomRightUV.y };
        uv.TopRight   = { bottomRightUV.x, topLeftUV.y };

        return uv;
    }
}