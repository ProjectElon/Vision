#pragma once

#include "Vision/Core/Defines.h"
#include "Vision/Assets/Assets.h"
#include "Vision/Renderer/RendererTypes.h"

#include <stb_truetype.h>

namespace Vision
{
    struct BitmapFont
    {
        stbtt_packedchar Glyphs[256];

        Texture Atlas = {};

        uint8* Buffer = nullptr;

        uint32  SizeInPixels = 16;

        char FirstCharacter = ' ';
        char LastCharacter = '~';
    };

    void InitBitmapFont(BitmapFont* font,
                        uint8* fontBuffer,
                        uint32 SizeInPixels,
                        char firstCharacter = ' ',
                        char lastCharacter = '~');

    void UninitBitmapFont(BitmapFont* font);
    void SetBitmapFontSize(BitmapFont* font, uint32 sizeInPixels);

    AssetLoadingData LoadBitmapFont(const std::string& fontpath);
    void UnloadBitmapFont(Asset* fontAsset);
}
