#include "pch.h"

#include "Vision/Renderer/TextureAtlas.h"

namespace Vision
{
    static void InitTextureAtlas(TextureAtlas* atlas,
                                 AssetID textureID,
                                 uint32 rectCount)
    {
        VnCoreAssert(atlas->UVRects == nullptr && rectCount > 0, "invalid texture atlas allocation");

        atlas->TextureID = textureID;
        atlas->RectCount = rectCount;

        atlas->UVRects = new UVRect[rectCount];
    }

    static void UninitTextureAtlas(TextureAtlas* atlas)
    {
        VnCoreAssert(atlas->UVRects != nullptr, "can't free a nullptr");

        delete[] atlas->UVRects;

        atlas->UVRects   = nullptr;
        atlas->RectCount = 0;
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



    TextureAtlasRects::TextureAtlasRects(AssetID textureID,
                                         uint32 rectCount,
                                         TextureRect* textureRects)
    {
        Init(textureID, rectCount, textureRects);
    }

    TextureAtlasRects::~TextureAtlasRects()
    {
        Uninit();
    }

    void TextureAtlasRects::Init(AssetID textureID,
                                 uint32 rectCount,
                                 TextureRect* textureRects)
    {
        InitTextureAtlas(&Atlas,
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
                                       &Atlas.UVRects[rectIndex]);
        }
    }



    TextureAtlasGrid::TextureAtlasGrid(AssetID textureID,
                                       uint32 cellWidth,
                                       uint32 cellHeight)
    {
        Init(textureID, cellWidth, cellHeight);
    }

    TextureAtlasGrid::~TextureAtlasGrid()
    {
        Uninit();
    }

    void TextureAtlasRects::Uninit()
    {
        UninitTextureAtlas(&Atlas);
    }

    void TextureAtlasGrid::Init(AssetID textureID,
                                uint32 cellWidth,
                                uint32 cellHeight)
    {
        Texture* texture = Assets::GetTexture(textureID);

        CellWidth  = cellWidth;
        CellHeight = cellHeight;

        Rows = texture->Height / CellHeight;
        Cols = texture->Width / CellWidth;

        uint32 rectCount = Rows * Cols;

        InitTextureAtlas(&Atlas,
                         textureID,
                         rectCount);

        for (uint32 row = 0; row < Rows; ++row)
        {
            for (uint32 col = 0; col < Cols; ++col)
            {
                TextureRect currentTextureRect =
                {
                    col * cellWidth,
                    row * cellHeight,
                    cellWidth,
                    cellHeight
                };

                uint32 currentTextureRectIndex = row * Cols + col;
                ConvertTextureRectToUVRect(&currentTextureRect,
                                           texture->Width,
                                           texture->Height,
                                           &Atlas.UVRects[currentTextureRectIndex]);
            }
        }
    }

    void TextureAtlasGrid::Uninit()
    {
        UninitTextureAtlas(&Atlas);
    }
}