#pragma once

#include "Vision/Core/Defines.h"

#include <functional>

namespace Vision
{
    enum class AssetState : uint8
    {
        Unloaded,
        Pending,
        Loaded,
        Reloading
    };

    struct AssetLoadingData
    {
        void* Memory = nullptr;

        uint64 SizeInBytes      = 0;
        uint64 TotalSizeInBytes = 0;
    };

    struct Asset
    {
        void* Memory = nullptr;

        AssetState State = AssetState::Unloaded;

        uint32 RefCount = 0;
        uint64 SizeInBytes = 0;
        uint64 TotalSizeInBytes = 0;

        uint32 AssetInfoIndex = 0;
        std::string Path;
    };

    using AssetID         = uint32;
    using AssetRigistry   = std::unordered_map<std::string, AssetID>;
    using AssetLoaderFn   = std::function<AssetLoadingData(const std::string& assetpath)>;
    using AssetUnloaderFn = std::function<void(Asset* asset)>;

    struct AssetInfo
    {
        std::string     Type;
        AssetLoaderFn   LoaderFn;
        AssetUnloaderFn UnloaderFn;
        AssetRigistry   Rigistry;
    };

    using AssetInfoMap = std::unordered_map<std::string, uint32>;
    using AssetInfoList = std::vector<AssetInfo>;
    using AssetList = std::vector<Asset>;

    struct AssetsStorage
    {
        uint64 TotalMemoryUsed = 0;

        AssetInfoMap  AssetInfoMap;
        AssetInfoList AssetInfoList;

        AssetList Assets;
    };

    struct Shader;
    struct Texture;
    struct Scene;
    struct BitmapFont;

    struct Assets
    {
        static AssetsStorage AssetsStorage;

        static void Initialize();
        static void Shutdown();

        static void RigisterAsset(const std::string& assetType,
                                  uint32 extensionCount,
                                  std::string* extensions,
                                  AssetLoaderFn loaderFn,
                                  AssetUnloaderFn unloaderFn);

        static bool IsExtensionSupported(const std::string& extension);
        static bool IsExtensionOfAssetType(const std::string& extension,
                                           const std::string& assetType);

        static AssetInfo& GetAssetInfo(const std::string& extension);
        static AssetInfo& GetAssetInfo(AssetID assetID);

        static AssetID RequestAsset(const std::string& assetpath);

        static AssetID GetAssetID(const std::string& assetpath);
        static const Asset& GetAsset(AssetID assetID);

        static Texture* GetTexture(AssetID textureAssetID);
        static Shader* GetShader(AssetID shaderAssetID);
        static Scene* GetScene(AssetID sceneAssetID);
        static BitmapFont* GetBitmapFont(AssetID bitmapFontAssetID);

        static void ReleaseAsset(AssetID assetID);
        static void ReleaseAsset(const std::string& assetpath);

        static void ReloadAsset(const std::string& assetpath);

    private:
        static uint32 GetAssetInfoIndex(const std::string& assetType);
    };

    AssetLoadingData LoadTexture(const std::string& texturepath);
    void UnloadTexture(Asset* textureAsset);

    AssetLoadingData LoadShader(const std::string& shaderpath);
    void UnloadShader(Asset* shader);

    AssetLoadingData LoadScene(const std::string& scenepath);
    void UnloadScene(Asset* sceneAsset);
}