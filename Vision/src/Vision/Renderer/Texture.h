#pragma once

#include "Vision/Core/Common.h"
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
        RGBA,
        Font
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

        FilterMode  Filter = FilterMode::Point;
        PixelFormat Format = PixelFormat::RGBA;

        Texture() = default;
        Texture(void* pixels, uint32 width, uint32 height, PixelFormat format);

        ~Texture();

        void Init(void* pixels, uint32 width, uint32 height, PixelFormat format);
        void Uninit();

        void Bind(uint32 textureSlot) const;
        void UnBind();

        void SetWrapMode(WrapMode wrapModeX, WrapMode wrapModeY);
        void SetFilterMode(FilterMode filter);
    };

    AssetLoadingData LoadTexture(const std::string& texturepath);
    void UnloadTexture(Asset* textureAsset);
}