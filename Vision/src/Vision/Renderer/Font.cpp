#include "pch.h"
#include "Vision/Renderer/Font.h"
#include "Vision/IO/File.h"

#define STB_TRUETYPE_IMPLEMENTATION
#include <stb_truetype.h>

namespace Vision
{

    BitmapFont::BitmapFont(uint8* fontBuffer,
                           uint32 sizeInPixels,
                           char firstCharacter,
                           char lastCharacter)
    {
        Init(fontBuffer,
             sizeInPixels,
             firstCharacter,
             lastCharacter);
    }

    BitmapFont::~BitmapFont()
    {
        Uninit();
    }

    void BitmapFont::Init(uint8* fontBuffer,
                          uint32 sizeInPixels,
                          char firstCharacter,
                          char lastCharacter)
    {
        SizeInPixels   = sizeInPixels;
        FirstCharacter = firstCharacter;
        LastCharacter  = lastCharacter;
        Buffer         = fontBuffer;

        uint32 characterCount = (lastCharacter - firstCharacter) + 1;

        uint32 textureAtlasWidth  = 1024;
        uint32 textureAtlasHeight = 1024;

        uint32 overSampleX = 2;
        uint32 overSampleY = 2;

        uint8* atlasBitmap = new uint8[textureAtlasWidth * textureAtlasHeight];

        stbtt_pack_context context;

        if (!stbtt_PackBegin(&context,
                             atlasBitmap,
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
                                 sizeInPixels,
                                 firstCharacter,
                                 characterCount,
                                 Glyphs))
        {
            VnCoreTrace("failed to pack font");
        }

        stbtt_PackEnd(&context);

        Atlas.Init(atlasBitmap,
                   textureAtlasWidth,
                   textureAtlasHeight,
                   PixelFormat::Font);

        Atlas.SetFilterMode(FilterMode::Bilinear);

        delete[] atlasBitmap;
    }

    void BitmapFont::Uninit()
    {
        Atlas.Uninit();
        delete[] Buffer;
    }

    void BitmapFont::SetSize(uint32 sizeInPixels)
    {
        Atlas.Uninit();
        Init(Buffer,
             sizeInPixels,
             FirstCharacter,
             LastCharacter);
    }

    AssetLoadingData LoadBitmapFont(const std::string& fontpath)
    {
        FileStream handle = File::Open(fontpath,
                                       FileMode::Read,
                                       true);

        uint8* fontBuffer = new uint8[handle.SizeInBytes];
        File::Read(handle, fontBuffer, handle.SizeInBytes);
        File::Close(handle);

        BitmapFont* font = new BitmapFont(fontBuffer, 16);

        AssetLoadingData fontAsset;
        fontAsset.Memory = font;
        fontAsset.SizeInBytes = sizeof(BitmapFont);
        fontAsset.TotalSizeInBytes = sizeof(BitmapFont);

        return fontAsset;
    }

    void UnloadBitmapFont(Asset* fontAsset)
    {
        BitmapFont* font = (BitmapFont*)fontAsset->Memory;
        delete font;
    }
}