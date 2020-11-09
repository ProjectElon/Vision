#include "pch.h"
#include "Vision/IO/Assets.h"
#include "Vision/IO/FileSystem.h"
#include "Vision/Renderer/Texture2D.h"
#include "Vision/Renderer/Shader.h"
#include "Vision/Renderer/OpenGL/OpenGLTexture2D.h"
#include "Vision/Renderer/OpenGL/OpenGLShader.h"

#define MaxAssetExtensionCount 128

namespace Vision
{
    // @Harlequin: We should return a string hash repersting the asset id

    uint64 AssetManager::TotalMemoryUsed = 0;

    using AssetInfoMap = std::unordered_map<std::string, AssetInfo>;
    using AssetExtensionMap = std::unordered_map<std::string, std::string>;

    static AssetInfoMap      InternalAssetInfoMap;
    static AssetExtensionMap InternalExtensionMap;

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
                      LoadTexture);

        std::string shaderExtensions[] =
        {
            "glsl"
        };

        RigisterAsset("Shader",
                      ArrayCount(shaderExtensions),
                      shaderExtensions,
                      LoadShader);
    }

    void AssetManager::Shutdown()
    {
    }

    void AssetManager::RigisterAsset(const std::string& type,
                                     uint32 extensionCount,
                                     std::string* extensions,
                                     AssetLoaderFn loaderFn)
    {
        AssetInfo& assetInfo = InternalAssetInfoMap[type];
        VN_CORE_ASSERT((assetInfo.ExtensionCount + assetInfo.ExtensionCount) <= MaxAssetExtensionCount, "too much extensions");

        assetInfo.LoaderFn = loaderFn;

        for (uint32 extensionIndex = 0;
             extensionIndex < extensionCount;
             ++extensionIndex)
        {
            const std::string& extension = extensions[extensionIndex];
            assetInfo.Extensions[extensionIndex + assetInfo.ExtensionCount] = extension;
            InternalExtensionMap[extension] = type;
        }

        assetInfo.ExtensionCount += extensionCount;
    }

    const AssetInfo& AssetManager::GetAssetInfo(const std::string& type)
    {
        auto assetInfoIter = InternalAssetInfoMap.find(type);
        VN_CORE_ASSERT(assetInfoIter != InternalAssetInfoMap.end(), "unsuppored asset type");
        return assetInfoIter->second;
    }

    bool AssetManager::IsExtensionSupported(const AssetInfo& assetInfo, const std::string& extension)
    {
        bool foundExtension = false;

        for (uint32 extensionIndex = 0;
             extensionIndex < assetInfo.ExtensionCount;
             ++extensionIndex)
        {
            if (assetInfo.Extensions[extensionIndex] == extension)
            {
                foundExtension = true;
                break;
            }
        }

        return foundExtension;
    }

    Asset* AssetManager::QueryAssetType(const std::string& type,
                                        const std::string& assetpath)
    {
        AssetInfo& assetInfo = InternalAssetInfoMap[type];
        AssetRigistry& rigistry = assetInfo.Rigistry;

        auto assetIter = rigistry.find(assetpath);

        Asset* asset = nullptr;

        if (assetIter == rigistry.end())
        {
            std::string extension = FileSystem::GetFileExtension(assetpath, false);
            bool supported = IsExtensionSupported(assetInfo, extension);

            if (supported)
            {
                asset = &rigistry[assetpath];            }
            else
            {
                VN_CORE_INFO("unsupported extension {0} for asset of type {1}", extension, type);
            }
        }
        else
        {
            asset = &assetIter->second;
            VN_CORE_INFO("retrieving {0} asset: {1} from asset rigistry", type, assetpath);
        }

        if (!asset->Memory)
        {
            *asset = assetInfo.LoaderFn(assetpath);

            if (!asset->Memory)
            {
                VN_CORE_INFO("unable to load {0} asset: {1}", type, assetpath);
            }
            else
            {
                TotalMemoryUsed += asset->SizeInBytes;
                VN_CORE_INFO("loaded {0} asset: {1} and adding to asset rigistry", type, assetpath);
            }
        }

        return asset;
    }

    void AssetManager::ReleaseAssetType(const std::string& type,
                                        const std::string& assetpath)
    {
        AssetInfo& assetInfo = InternalAssetInfoMap[type];
        AssetRigistry& rigistry = assetInfo.Rigistry;

        auto assetIter = rigistry.find(assetpath);

        if (assetIter != rigistry.end())
        {
            Asset& asset = assetIter->second;
            TotalMemoryUsed -= asset.SizeInBytes;

            // @Hack: really c++
            // Need a Reload Function callback per asset

            if (type == "Shader")
            {
                delete (Shader*)asset.Memory;
            }
            else if (type == "Texture")
            {
                delete (Texture2D*)asset.Memory;
            }

            asset.Memory = nullptr;
        }
        else
        {
            VN_CORE_INFO("can't find {0} asset: {1} in asset rigistry", type, assetpath);
        }
    }

    Asset* AssetManager::QueryAsset(const std::string& assetpath)
    {
        std::string extension = FileSystem::GetFileExtension(assetpath, false);

        auto extensionIter = InternalExtensionMap.find(extension);
        VN_CORE_ASSERT(extensionIter != InternalExtensionMap.end(), "unsupported asset extension");
        VN_CORE_ASSERT(FileSystem::FileExists(assetpath), "asset file not found");

        const std::string& assetType = extensionIter->second;
        return QueryAssetType(assetType, assetpath);
    }

    void AssetManager::ReleaseAsset(const std::string& assetpath)
    {
        std::string extension = FileSystem::GetFileExtension(assetpath, false);

        auto extensionIter = InternalExtensionMap.find(extension);
        VN_CORE_ASSERT(extensionIter != InternalExtensionMap.end(), "unsupported asset extension");

        const std::string& assetType = extensionIter->second;

        return ReleaseAssetType(assetType, assetpath);
    }

    Asset AssetManager::LoadTexture(const std::string& texturepath)
    {
        Asset asset;
        asset.Memory = Texture2D::CreateFromFile(texturepath);
        asset.SizeInBytes = sizeof(OpenGLTexture2D);
        return asset;
    }

    Asset AssetManager::LoadShader(const std::string& shaderpath)
    {
        Asset asset;
        asset.Memory = Shader::CreateFromFile(shaderpath);
        asset.SizeInBytes = sizeof(OpenGLShader);
        return asset;
    }
}