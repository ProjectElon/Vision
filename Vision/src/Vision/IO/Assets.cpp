#include "pch.h"
#include "Vision/IO/Assets.h"
#include "Vision/IO/FileSystem.h"

#include "Vision/Entity/Scene.h"
#include "Vision/Renderer/Texture.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/Font.h"

namespace Vision
{
    void Assets::Init()
    {
        AssetsStorage.Assets.resize(1);

        std::string textureExtensions[] =
        {
            "png",
            "jpeg",
            "tga",
            "bmp",
            "psd"
        };

        RigisterAsset("Texture",
                      VnArrayCount(textureExtensions),
                      textureExtensions,
                      LoadTexture,
                      UnloadTexture);

        std::string shaderExtensions[] =
        {
            "glsl"
        };

        RigisterAsset("Shader",
                      VnArrayCount(shaderExtensions),
                      shaderExtensions,
                      LoadShader,
                      UnloadShader);

        std::string sceneExtensions[] =
        {
            "vscene"
        };

        RigisterAsset("Scene",
                      VnArrayCount(sceneExtensions),
                      sceneExtensions,
                      LoadScene,
                      UnloadScene);

        std::string fontExtensions[] =
        {
            "ttf"
        };

        RigisterAsset("BitmapFont",
                      VnArrayCount(fontExtensions),
                      fontExtensions,
                      LoadBitmapFont,
                      UnloadBitmapFont);
    }

    void Assets::Shutdown()
    {
    }

    void Assets::RigisterAsset(const std::string& assetType,
                                     uint32 extensionCount,
                                     std::string* extensions,
                                     AssetLoaderFn loaderFn,
                                     AssetUnloaderFn unloaderFn)
    {
        AssetInfoMap& assetInfoMap = AssetsStorage.AssetInfoMap;
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;

        uint32 assetInfoIndex = assetInfoList.size();
        assetInfoList.resize(assetInfoIndex + 1);

        AssetInfo& assetInfo = assetInfoList[assetInfoIndex];
        assetInfo.Type = assetType;

        assetInfo.LoaderFn   = loaderFn;
        assetInfo.UnloaderFn = unloaderFn;

        for (uint32 extensionIndex = 0;
             extensionIndex < extensionCount;
             ++extensionIndex)
        {
            const std::string& extension = extensions[extensionIndex];
            assetInfoMap[extension] = assetInfoIndex;
        }
    }

    uint32 Assets::GetAssetInfoIndex(const std::string& extension)
    {
        AssetInfoMap& assetInfoMap = AssetsStorage.AssetInfoMap;
        auto assetInfoIter = assetInfoMap.find(extension);
        VnCoreAssert(assetInfoIter != assetInfoMap.end(), "unsuppored asset type");
        uint32 assetInfoIndex = assetInfoIter->second;
        return assetInfoIndex;
    }

    AssetInfo& Assets::GetAssetInfo(const std::string& extension)
    {
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        uint32 assetInfoIndex = GetAssetInfoIndex(extension);
        return assetInfoList[assetInfoIndex];
    }

    AssetInfo& Assets::GetAssetInfo(AssetID assetID)
    {
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetList& assets = AssetsStorage.Assets;

        VnCoreAssert(assetID >= 0 && assetID < assetInfoList.size(), "invalid asset id");
        Asset& asset = assets[assetID];
        return assetInfoList[asset.AssetInfoIndex];
    }

    bool Assets::IsExtensionSupported(const std::string& extension)
    {
        AssetInfoMap& assetInfoMap = AssetsStorage.AssetInfoMap;
        auto assetInfoIter = assetInfoMap.find(extension);
        return assetInfoIter != assetInfoMap.end();
    }

    AssetID Assets::RequestAsset(const std::string& assetpath)
    {
        VnCoreAssert(FileSystem::FileExists(assetpath), "can't find asset: {0}");

        std::string extension = FileSystem::GetFileExtension(assetpath, false);

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;

        uint32 assetInfoIndex = GetAssetInfoIndex(extension);
        AssetInfo& assetInfo = assetInfoList[assetInfoIndex];

        AssetRigistry& rigistry = assetInfo.Rigistry;

        auto assetIDIter = rigistry.find(assetpath);

        AssetID assetID;

        if (assetIDIter == rigistry.end())
        {
            assetID = AssetsStorage.Assets.size();
            AssetsStorage.Assets.resize(assetID + 1);

            AssetsStorage.Assets[assetID].Path = assetpath;
            AssetsStorage.Assets[assetID].AssetInfoIndex = assetInfoIndex;

            rigistry.emplace(assetpath, assetID);
        }
        else
        {
            assetID = assetIDIter->second;
        }

        if (AssetsStorage.Assets[assetID].State == AssetState::Unloaded)
        {
            AssetsStorage.Assets[assetID].State = AssetState::Pending;

            AssetLoadingData assetData = assetInfo.LoaderFn(assetpath);

            if (assetData.Memory && assetData.SizeInBytes)
            {
                AssetsStorage.Assets[assetID].Memory = assetData.Memory;
                AssetsStorage.Assets[assetID].SizeInBytes = assetData.SizeInBytes;
                AssetsStorage.Assets[assetID].TotalSizeInBytes = assetData.TotalSizeInBytes;

                AssetsStorage.Assets[assetID].State = AssetState::Loaded;
                AssetsStorage.TotalMemoryUsed += assetData.TotalSizeInBytes;

                VnCoreInfo("loaded {0} asset: {1}", assetInfo.Type, assetpath);
            }
            else
            {
                VnCoreInfo("unable to load {0} asset: {1}", assetInfo.Type, assetpath);
            }
        }

        AssetsStorage.Assets[assetID].RefCount++;

        return assetID;
    }

    AssetID Assets::GetAssetID(const std::string& assetpath)
    {
        VnCoreAssert(FileSystem::FileExists(assetpath), "can't find asset: {0}");
        std::string extension = FileSystem::GetFileExtension(assetpath, false);
        uint32 assetInfoIndex = GetAssetInfoIndex(extension);
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[assetInfoIndex];
        AssetRigistry& rigistry = assetInfo.Rigistry;

        auto assetIDIter = rigistry.find(assetpath);
        VnCoreAssert(assetIDIter != rigistry.end(), "can't find asset in rigistry");

        AssetID assetID = assetIDIter->second;
        return assetID;
    }

    const Asset& Assets::GetAsset(AssetID assetID)
    {
        VnCoreAssert(assetID < AssetsStorage.Assets.size(), "invalid asset id");
        return AssetsStorage.Assets[assetID];
    }

    Texture* Assets::GetTexture(AssetID assetID)
    {
        const Asset& asset = GetAsset(assetID);
        VnCoreAssert(asset.State == AssetState::Loaded, "can't use an unloaded asset");

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];
        VnCoreAssert(assetInfo.Type == "Texture", "asset type mismatch");

        return static_cast<Texture*>(asset.Memory);
    }

    Shader* Assets::GetShader(AssetID shaderAssetID)
    {
        const Asset& asset = GetAsset(shaderAssetID);
        VnCoreAssert(asset.State == AssetState::Loaded, "can't use an unloaded asset");

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];
        VnCoreAssert(assetInfo.Type == "Shader", "asset type mismatch");

        return static_cast<Shader*>(asset.Memory);
    }

    Scene* Assets::GetScene(AssetID sceneAssetID)
    {
        const Asset& asset = GetAsset(sceneAssetID);
        VnCoreAssert(asset.State == AssetState::Loaded, "can't use an unloaded asset");

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];
        VnCoreAssert(assetInfo.Type == "Scene", "asset type mismatch");

        return static_cast<Scene*>(asset.Memory);
    }

    BitmapFont* Assets::GetBitmapFont(AssetID bitmapFontAssetID)
    {
        const Asset& asset = GetAsset(bitmapFontAssetID);
        VnCoreAssert(asset.State == AssetState::Loaded, "can't use an unloaded asset");

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];
        VnCoreAssert(assetInfo.Type == "BitmapFont", "asset type mismatch");

        return static_cast<BitmapFont*>(asset.Memory);
    }

    void Assets::ReleaseAsset(AssetID assetID)
    {
        AssetList& assets = AssetsStorage.Assets;
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        VnCoreAssert(assetID < assets.size(), "invalid asset id");

        Asset& asset = assets[assetID];
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];

        if (asset.RefCount > 0)
        {
            asset.RefCount--;

            if (asset.RefCount == 0)
            {
                VnCoreAssert(asset.State != AssetState::Unloaded, "invalid asset state");

                assetInfo.UnloaderFn(&asset);

                asset.State = AssetState::Unloaded;
                AssetsStorage.TotalMemoryUsed -= asset.TotalSizeInBytes;

                asset.Memory = nullptr;
                asset.SizeInBytes = 0;
                asset.TotalSizeInBytes = 0;

                VnCoreInfo("unloading {0} asset: {1}", assetInfo.Type, asset.Path);
            }
        }
        else
        {
            VnCoreInfo("{0} asset: {1} already unloaded", assetInfo.Type, asset.Path);
            return;
        }
    }

    void Assets::ReleaseAsset(const std::string& assetpath)
    {
        std::string extension = FileSystem::GetFileExtension(assetpath, false);

        AssetInfo& assetInfo = GetAssetInfo(extension);
        AssetRigistry& rigistry = assetInfo.Rigistry;

        auto assetIter = rigistry.find(assetpath);

        if (assetIter != rigistry.end())
        {
            AssetID assetID = assetIter->second;
            ReleaseAsset(assetID);
        }
        else
        {
            VnCoreInfo("unable to release {0} asset: {1}", assetInfo.Type, assetpath);
        }
    }

    void Assets::ReloadAsset(const std::string& assetpath)
    {
        std::string extension = FileSystem::GetFileExtension(assetpath, false);

        AssetInfo& assetInfo = GetAssetInfo(extension);
        AssetList& assets = AssetsStorage.Assets;

        AssetRigistry& rigistry = assetInfo.Rigistry;

        auto assetIter = rigistry.find(assetpath);

        if (assetIter != rigistry.end())
        {
            AssetID assetID = assetIter->second;
            Asset& asset = assets[assetID];

            if (asset.State == AssetState::Loaded)
            {
                asset.State = AssetState::Reloading;

                AssetLoadingData newAsset = assetInfo.LoaderFn(assetpath);

                if (newAsset.Memory && newAsset.TotalSizeInBytes)
                {
                    AssetsStorage.TotalMemoryUsed -= asset.TotalSizeInBytes;
                    AssetsStorage.TotalMemoryUsed += newAsset.TotalSizeInBytes;

                    assetInfo.UnloaderFn(&asset);

                    asset.Memory = newAsset.Memory;
                    asset.SizeInBytes = newAsset.SizeInBytes;
                    asset.TotalSizeInBytes = newAsset.TotalSizeInBytes;

                    asset.State = AssetState::Loaded;

                    VnCoreInfo("reloading {0} asset: {1}", assetInfo.Type, assetpath);
                }
                else
                {
                    /*
                    @(Harlequin):
                        what we should do if the reloading failed ?
                        right now we just leaving the old value and notifing
                        maybe we want to load a default value instead
                        like a pink texture for example
                    */
                    VnCoreInfo("unable to reload {0} asset: {1}", assetInfo.Type, assetpath);
                }
            }
        }
    }

    AssetsStorage Assets::AssetsStorage;
}