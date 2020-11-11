#include "pch.h"
#include "Vision/IO/Assets.h"
#include "Vision/IO/FileSystem.h"
#include "Vision/Renderer/Texture2D.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/OpenGL/OpenGLTexture2D.h"
#include "Vision/Renderer/OpenGL/OpenGLShader.h"

namespace Vision
{
    AssetManagerStorage AssetManager::AssetsStorage;

    void AssetManager::Init()
    {
        std::string textureExtensions[] =
        {
            "png",
            "jpeg",
            "tga",
            "bmp",
            "psd"
        };

        RigisterAsset("Texture",
                      ArrayCount(textureExtensions),
                      textureExtensions,
                      LoadTexture,
                      UnloadTexture);

        std::string shaderExtensions[] =
        {
            "glsl"
        };

        RigisterAsset("Shader",
                      ArrayCount(shaderExtensions),
                      shaderExtensions,
                      LoadShader,
                      UnloadShader);
    }

    void AssetManager::Shutdown()
    {
    }

    void AssetManager::RigisterAsset(const std::string& assetType,
                                     uint32 extensionCount,
                                     std::string* extensions,
                                     AssetLoaderFn loaderFn,
                                     AssetUnloaderFn unloaderFn)
    {
        AssetInfoMap& assetInfoMap = AssetsStorage.AssetInfoMap;
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetList& assets = AssetsStorage.Assets;

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

    uint32 AssetManager::GetAssetInfoIndex(const std::string& extension)
    {
        AssetInfoMap& assetInfoMap = AssetsStorage.AssetInfoMap;
        auto assetInfoIter = assetInfoMap.find(extension);
        VN_CORE_ASSERT(assetInfoIter != assetInfoMap.end(), "unsuppored asset type");
        uint32 assetInfoIndex = assetInfoIter->second;
        return assetInfoIndex;
    }

    AssetInfo& AssetManager::GetAssetInfo(const std::string& extension)
    {
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        uint32 assetInfoIndex = GetAssetInfoIndex(extension);
        return assetInfoList[assetInfoIndex];
    }

    AssetInfo& AssetManager::GetAssetInfo(AssetID assetID)
    {
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetList& assets = AssetsStorage.Assets;

        VN_CORE_ASSERT(assetID >= 0 && assetID < assetInfoList.size(), "invalid asset id");
        Asset& asset = assets[assetID];
        return assetInfoList[asset.AssetInfoIndex];
    }

    bool AssetManager::IsExtensionSupported(const std::string& extension)
    {
        AssetInfoMap& assetInfoMap = AssetsStorage.AssetInfoMap;
        auto assetInfoIter = assetInfoMap.find(extension);
        return assetInfoIter != assetInfoMap.end();
    }

    AssetID AssetManager::RequestAsset(const std::string& assetpath)
    {
        VN_CORE_ASSERT(FileSystem::FileExists(assetpath), "can't find asset: {0}");

        std::string extension = FileSystem::GetFileExtension(assetpath, false);

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetList& assets = AssetsStorage.Assets;

        uint32 assetInfoIndex = GetAssetInfoIndex(extension);
        AssetInfo& assetInfo = assetInfoList[assetInfoIndex];

        AssetRigistry& rigistry = assetInfo.Rigistry;

        auto assetIter = rigistry.find(assetpath);
        AssetID assetID;
        Asset* asset;

        if (assetIter == rigistry.end())
        {
            assetID = assets.size();
            assets.resize(assetID + 1);

            asset = &assets[assetID];
            asset->Path = assetpath;
            asset->AssetInfoIndex = assetInfoIndex;

            rigistry.emplace(assetpath, assetID);
        }
        else
        {
            assetID = assetIter->second;
            asset = &assets[assetID];
        }

        if (asset->State == AssetState::Unloaded)
        {
            asset->State = AssetState::Pending;

            AssetLoadingData assetData = assetInfo.LoaderFn(assetpath);

            if (assetData.Memory && assetData.SizeInBytes)
            {
                asset->Memory = assetData.Memory;
                asset->SizeInBytes = assetData.SizeInBytes;
                asset->TotalSizeInBytes = assetData.TotalSizeInBytes;

                asset->State = AssetState::Loaded;
                AssetsStorage.TotalMemoryUsed += asset->TotalSizeInBytes;

                VN_CORE_INFO("loaded {0} asset: {1}", assetInfo.Type, assetpath);
            }
            else
            {
                VN_CORE_INFO("unable to load {0} asset: {1}", assetInfo.Type, assetpath);
            }
        }

        asset->RefCount++;

        return assetID;
    }

    const Asset& AssetManager::GetAsset(AssetID assetID)
    {
        VN_CORE_ASSERT(assetID >= 0 && assetID < AssetsStorage.Assets.size(), "invalid asset id");
        return AssetsStorage.Assets[assetID];
    }

    Texture2D* AssetManager::GetTexture(AssetID assetID)
    {
        const Asset& asset = GetAsset(assetID);
        VN_CORE_ASSERT(asset.State == AssetState::Loaded, "can't use an unloaded asset");

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];
        VN_CORE_ASSERT(assetInfo.Type == "Texture", "asset type mismatch");

        return (Texture2D*)asset.Memory;
    }

    Shader* AssetManager::GetShader(AssetID assetID)
    {
        const Asset& asset = GetAsset(assetID);
        VN_CORE_ASSERT(asset.State == AssetState::Loaded, "can't use an unloaded asset");

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];
        VN_CORE_ASSERT(assetInfo.Type == "Shader", "asset type mismatch");

        return (Shader*)asset.Memory;
    }

    void AssetManager::ReleaseAsset(AssetID assetID)
    {
        AssetList& assets = AssetsStorage.Assets;
        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        VN_CORE_ASSERT(assetID >= 0 && assetID < assets.size(), "invalid asset id");

        Asset& asset = assets[assetID];
        asset.RefCount--;

        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];

        if (!asset.RefCount)
        {
            VN_CORE_ASSERT(asset.State != AssetState::Unloaded, "invalid asset state");

            assetInfo.UnloaderFn(&asset);

            asset.State = AssetState::Unloaded;
            AssetsStorage.TotalMemoryUsed -= asset.TotalSizeInBytes;

            asset.Memory = nullptr;
            asset.SizeInBytes = 0;
            asset.TotalSizeInBytes = 0;

            VN_CORE_INFO("unloading {0} asset: {1}", assetInfo.Type, asset.Path);
        }
        else
        {
            VN_CORE_INFO("{0} asset: {1} already unloaded", assetInfo.Type, asset.Path);
        }
    }

    void AssetManager::ReleaseAsset(const std::string& assetpath)
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
            VN_CORE_INFO("unable to release {0} asset: {1}", assetInfo.Type, assetpath);
        }
    }

    void AssetManager::ReloadAsset(const std::string& assetpath)
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

                    VN_CORE_INFO("reloading {0} asset: {1}", assetInfo.Type, assetpath);
                }
                else
                {
                    /*
                    @Harlequin: what we should do if the reloading failed
                    right now we just leaving the old value ????
                    */
                    VN_CORE_INFO("unable to reload {0} asset: {1}", assetInfo.Type, assetpath);
                }
            }
        }
    }

    AssetLoadingData AssetManager::LoadTexture(const std::string& texturepath)
    {
        AssetLoadingData textureAsset;
        textureAsset.Memory = Texture2D::CreateFromFile(texturepath);
        // Note: We don't count the gpu used memory
        textureAsset.SizeInBytes = sizeof(OpenGLTexture2D) + sizeof(Asset);
        textureAsset.TotalSizeInBytes = sizeof(OpenGLTexture2D) + sizeof(Asset);
        return textureAsset;
    }

    void AssetManager::UnloadTexture(Asset* texture)
    {
        delete (Texture2D*)texture->Memory;
    }

    AssetLoadingData AssetManager::LoadShader(const std::string& shaderpath)
    {
        AssetLoadingData shaderAsset;
        shaderAsset.Memory = Shader::CreateFromFile(shaderpath);
        // Note: We don't count the gpu used memory
        shaderAsset.SizeInBytes = sizeof(OpenGLShader) + sizeof(Asset);
        shaderAsset.TotalSizeInBytes = sizeof(OpenGLShader) + sizeof(Asset);
        return shaderAsset;
    }

    void AssetManager::UnloadShader(Asset* shader)
    {
        delete (Shader*)shader->Memory;
    }
}