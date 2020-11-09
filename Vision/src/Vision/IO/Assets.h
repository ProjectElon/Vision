#pragma once

#include "Vision/Core/Base.h"
#include <functional>

namespace Vision
{
    #define MaxAssetExtensionCount 128

    struct Asset
    {
        void*  Memory = nullptr;
        uint32 SizeInBytes = 0;
    };

    using AssetRigistry = std::unordered_map<std::string, Asset>;
    using AssetLoaderFn = std::function<Asset(const std::string& assetpath)>;

    struct AssetInfo
    {
        uint32        ExtensionCount = 0;
        std::string   Extensions[MaxAssetExtensionCount];

        AssetLoaderFn LoaderFn;
        AssetRigistry Rigistry;
    };

    class AssetManager
    {
    public:
        static uint64 TotalMemoryUsed; //@Don't Change

        static void Init();
        static void Shutdown();

        static void RigisterAsset(const std::string& type,
                                  uint32 extensionCount,
                                  std::string* extensions,
                                  AssetLoaderFn loaderFn);

        static const AssetInfo& GetAssetInfo(const std::string& type);
        static bool IsExtensionSupported(const AssetInfo& assetInfo, const std::string& extension);

        static Asset* QueryAsset(const std::string& assetpath);
        static void ReleaseAsset(const std::string& assetpath);

        static Asset* QueryAssetType(const std::string& type, const std::string& assetpath);
        static void ReleaseAssetType(const std::string& type, const std::string& assetpath);

        static Asset LoadTexture(const std::string& texturepath);
        static Asset LoadShader(const std::string& shaderpath);
    };
}