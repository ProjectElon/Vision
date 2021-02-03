#pragma once

#include "Vision/Core/Base.h"
#include "Vision/IO/Assets.h"
#include "Vision/Renderer/Texture.h"
#include <stb_truetype.h>

namespace Vision
{
    struct BitmapFont
    {
        stbtt_packedchar Glyphs[256];

        Texture Atlas;

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

    void SetFontSize(BitmapFont* font, uint32 SizeInPixels);

    AssetLoadingData LoadBitmapFont(const std::string& fontpath);
    void UnloadBitmapFont(Asset* fontAsset);
}
