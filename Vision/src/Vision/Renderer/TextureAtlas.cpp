#include "pch.h"

#include "Vision/Renderer/TextureAtlas.h"

namespace Vision
{
    UVRect ConvertTextureRectToUVRect(const TextureRect& rect, uint32 textureWidth, uint32 textureHeight);
    void AllocateTextureAtlas(TextureAtlas* atlas, Texture2D* texture, uint32 rectCount);

    /*
        Case1: The Texture Atlas is divided into cells with varying sizes.
    */
    TextureAtlas CreateTextureAtlas(Texture2D* texture, uint32 rectCount, TextureRect* rects)
    {
        TextureAtlas atlas;

        AllocateTextureAtlas(&atlas, texture, rectCount);

        const TextureData& data = texture->GetData();

        uint32 textureWidth  = data.Width;
        uint32 textureHeight = data.Height;

        for (uint32 rectIndex = 0;
             rectIndex < rectCount;
             ++rectIndex)
        {
            atlas.Rects[rectIndex] = ConvertTextureRectToUVRect(rects[rectIndex], textureWidth, textureHeight);
        }

        return atlas;
    }

    /*
        Case2: The Texture Atlas is divided into cells with the same size
        Starting from top left (0, 0) going to bottom right (textureWidth, textureHeight)
        going cell by cell from left to right cycling.
    */

    TextureAtlas CreateTextureAtlas(Texture2D* texture, uint32 rectCount, uint32 cellWidth, uint32 cellHeight)
    {
        TextureAtlas atlas;

        AllocateTextureAtlas(&atlas, texture, rectCount);

        const TextureData& data = texture->GetData();

        uint32 textureWidth  = data.Width;
        uint32 textureHeight = data.Height;

        int32 cellX = 0;
        int32 cellY = 0;

        for (uint32 rectIndex = 0;
             rectIndex < rectCount;
             ++rectIndex)
        {
            TextureRect currentRect = { cellX, cellY, cellWidth, cellHeight };

            atlas.Rects[rectIndex] = ConvertTextureRectToUVRect(currentRect, textureWidth, textureHeight);

            cellX += cellWidth;

            if (cellX == textureWidth)
            {
                cellX = 0;
                cellY += cellHeight;
            }
        }

        return atlas;
    }

    void FreeTextureAtlas(TextureAtlas* atlas)
    {
        VN_CORE_ASSERT(atlas->Rects != nullptr, "can't free a nullptr");

        delete[] atlas->Rects;

        atlas->Texture   = nullptr;
        atlas->Rects     = nullptr;
        atlas->RectCount = 0;
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

    static void AllocateTextureAtlas(TextureAtlas* atlas, Texture2D* texture, uint32 rectCount)
    {
        VN_CORE_ASSERT(atlas->Rects == nullptr && texture != nullptr && rectCount > 0, "invalid texture atlas allocation");

        atlas->Texture   = texture;
        atlas->RectCount = rectCount;
        atlas->Rects     = new UVRect[rectCount];
    }
}