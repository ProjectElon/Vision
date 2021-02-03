#include "pch.h"
#include "Vision/Renderer/Font.h"
#include "Vision/IO/File.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace Vision
{
    void InitBitmapFont(BitmapFont* font,
                        uint8* fontBuffer,
                        uint32 SizeInPixels,
                        char firstCharacter,
                        char lastCharacter)
    {
        font->SizeInPixels   = SizeInPixels;
        font->FirstCharacter = firstCharacter;
        font->LastCharacter  = lastCharacter;
        font->Buffer         = fontBuffer;

        uint32 characterCount = (lastCharacter - firstCharacter) + 1;

        uint32 textureAtlasWidth  = 1024;
        uint32 textureAtlasHeight = 1024;

        uint32 overSampleX = 2;
        uint32 overSampleY = 2;

        uint8* AtlasBitmap = new uint8[textureAtlasWidth * textureAtlasHeight];

        stbtt_pack_context context;

        if (!stbtt_PackBegin(&context,
                             AtlasBitmap,
                             textureAtlasWidth,
                             textureAtlasHeight,
                             0,
                             1,
                             nullptr))
        {
            VnCoreTrace("failed to initialize font");
        }

        stbtt_PackSetOversampling(&context,
                                  overSampleX,
                                  overSampleY);

        if (!stbtt_PackFontRange(&context,
                                 fontBuffer,
                                 0,
                                 SizeInPixels,
                                 firstCharacter,
                                 characterCount,
                                 font->Glyphs))
        {
            VnCoreTrace("failed to pack font");
        }

        stbtt_PackEnd(&context);

        InitTexture(&font->Atlas,
                      AtlasBitmap,
                      textureAtlasWidth,
                      textureAtlasHeight,
                      PixelFormat::Font);

        SetTextureFilterMode(&font->Atlas, FilterMode::Bilinear);

        delete[] AtlasBitmap;
    }

    void UninitBitmapFont(BitmapFont* font)
    {
        UninitTexture(&font->Atlas);
        delete[] font->Buffer;
    }

    void SetFontSize(BitmapFont* font, uint32 SizeInPixels)
    {
        UninitTexture(&font->Atlas);
        InitBitmapFont(font, font->Buffer, SizeInPixels);
    }

    AssetLoadingData LoadBitmapFont(const std::string& fontpath)
    {
        FileStream handle = File::Open(fontpath,
                                       FileMode::Read,
                                       true);

        uint8* fontBuffer = new uint8[handle.SizeInBytes];
        File::Read(handle, fontBuffer, handle.SizeInBytes);
        File::Close(handle);

        BitmapFont* font = new BitmapFont;
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
    }
}