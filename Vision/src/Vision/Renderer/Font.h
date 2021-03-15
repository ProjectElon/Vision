#pragma once

#include "Vision/Core/Common.h"
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

        BitmapFont() = default;
        BitmapFont(uint8* fontBuffer,
                   uint32 SizeInPixels,
                   char firstCharacter = ' ',
                   char lastCharacter = '~');

        ~BitmapFont();

        void Init(uint8* fontBuffer,
                  uint32 SizeInPixels,
                  char firstCharacter = ' ',
                  char lastCharacter = '~');

        void Uninit();

        void SetSize(uint32 sizeInPixels);
    };

    AssetLoadingData LoadBitmapFont(const std::string& fontpath);
    void UnloadBitmapFont(Asset* fontAsset);
}
