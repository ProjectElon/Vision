#pragma once

#include "Vision/Core/Base.h"
#include "Vision/IO/Assets.h"

#include <glm/glm.hpp>

namespace Vision
{
    enum class WrapMode
    {
        Repeat,
        ClampToEdge
    };

    enum class FilterMode
    {
        Point,
        Bilinear
    };

    enum class PixelFormat
    {
        R,
        RGB,
        RGBA
    };

    struct TextureRect
    {
        int32 X = 0;
        int32 Y = 0;

        uint32 Width  = 0;
        uint32 Height = 0;
    };

    struct UVRect
    {
        glm::vec2 BottomLeft;
        glm::vec2 TopRight;
    };

    struct Texture
    {
        uint32 RendererID = 0;
        uint32 Width      = 0;
        uint32 Height     = 0;

        WrapMode WrapX = WrapMode::Repeat;
        WrapMode WrapY = WrapMode::Repeat;

        FilterMode FilterMode = FilterMode::Point;
    };

    void CreateTexture(Texture* texture,
                       void* data,
                       uint32 width,
                       uint32 height,
                       PixelFormat pixelFormat);

    void DestroyTexture(Texture* texture);

    void BindTexture(const Texture* texture, uint32 textureSlot);
    void UnBindTexture(const Texture* texture);

    void SetTextureWrapMode(Texture* texture, WrapMode wrapModeX, WrapMode wrapModeY);
    void SetTextureFilterMode(Texture* texture, FilterMode filterMode);

    AssetLoadingData LoadTexture(const std::string& texturepath);
    void UnloadTexture(Asset* textureAsset);
}