#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"

#include "Vision/Renderer/Font.h"
#include "Vision/Renderer/Renderer.h"
#include "Vision/IO/File.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace Vision
{
    void InitBitmapFont(BitmapFont* font,
                        uint8* fontBuffer,
                        uint32 sizeInPixels,
                        char firstCharacter,
                        char lastCharacter)
    {

        font->SizeInPixels   = sizeInPixels;
        font->FirstCharacter = firstCharacter;
        font->LastCharacter  = lastCharacter;
        font->Buffer         = fontBuffer;

        uint32 characterCount = (lastCharacter - firstCharacter) + 1;

        uint32 textureAtlasWidth  = 1024;
        uint32 textureAtlasHeight = 1024;

        uint32 overSampleX = 2;
        uint32 overSampleY = 2;

        uint8* alphaBitmap = new uint8[textureAtlasWidth * textureAtlasHeight];

        stbtt_pack_context packingContext;

        if (!stbtt_PackBegin(&packingContext,
                             alphaBitmap,
                             textureAtlasWidth,
                             textureAtlasHeight,
                             0,
                             1,
                             nullptr))
        {
            VnCoreTrace("failed to initialize font");
        }

        stbtt_PackSetOversampling(&packingContext,
                                  overSampleX,
                                  overSampleY);

        if (!stbtt_PackFontRange(&packingContext,
                                 fontBuffer,
                                 0,
                                 sizeInPixels,
                                 firstCharacter,
                                 characterCount,
                                 font->Glyphs))
        {
            VnCoreTrace("failed to pack font");
        }

        stbtt_PackEnd(&packingContext);

        uint32* atlasBitmap = new uint32[textureAtlasWidth * textureAtlasHeight];

        for (uint32 pixelIndex = 0;
             pixelIndex < textureAtlasWidth * textureAtlasHeight;
             ++pixelIndex)
        {
            uint32 alpha = alphaBitmap[pixelIndex];
            atlasBitmap[pixelIndex] = alpha | (alpha << 8) | (alpha << 16) | (alpha << 24);
        }

        Renderer::API.InitTexture(&font->Atlas,
                                  atlasBitmap,
                                  textureAtlasWidth,
                                  textureAtlasHeight,
                                  PixelFormat_RGBA,
                                  WrapMode_ClampToEdge,
                                  WrapMode_ClampToEdge,
                                  FilterMode_Bilinear);

        delete[] atlasBitmap;
        delete[] alphaBitmap;
    }

    void UninitBitmapFont(BitmapFont* font)
    {
        Renderer::API.UninitTexture(&font->Atlas);
        delete[] font->Buffer;
    }

    void SetBitmapFontSize(BitmapFont* font, uint32 sizeInPixels)
    {
        Renderer::API.UninitTexture(&font->Atlas);

        InitBitmapFont(font,
                       font->Buffer,
                       sizeInPixels,
                       font->FirstCharacter,
                       font->LastCharacter);
    }

    AssetLoadingData LoadBitmapFont(const std::string& fontpath)
    {
        FileStream handle = File::Open(fontpath,
                                       FileMode::Read,
                                       true);

        uint8* fontBuffer = new uint8[handle.SizeInBytes];
        File::Read(handle, fontBuffer, handle.SizeInBytes);
        File::Close(handle);

        BitmapFont* font = (BitmapFont*)_aligned_malloc(sizeof(BitmapFont), alignof(BitmapFont));
        InitBitmapFont(font, fontBuffer, 16);

        AssetLoadingData fontAsset;
        fontAsset.Memory = font;
        fontAsset.SizeInBytes = sizeof(BitmapFont);
        fontAsset.TotalSizeInBytes = sizeof(BitmapFont);

        return fontAsset;
    }

    void UnloadBitmapFont(Asset* fontAsset)
    {
        BitmapFont* font = (BitmapFont*)fontAsset->Memory;
        UninitBitmapFont(font);
        _aligned_free(font);
    }
}