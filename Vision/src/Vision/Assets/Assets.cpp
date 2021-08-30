#include "pch.hpp"
#include "Vision/Assets/Assets.h"
#include "Vision/Platform/FileSystem.h"

#include "Vision/Entity/Scene.h"

#include "Vision/Renderer/Renderer.h"
#include "Vision/Renderer/RendererTypes.h"

#include "Vision/Renderer/Font.h"
#include "Vision/Renderer/Mesh.h"
#include "Vision/Assets/ObjLoader.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Vision
{
    void Assets::Initialize()
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

        std::string meshExtensions[] =
        {
            "obj"
        };

        RigisterAsset("Mesh",
                      VnArrayCount(meshExtensions),
                      meshExtensions,
                      LoadMesh,
                      UnloadMesh);
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

    bool Assets::IsExtensionOfAssetType(const std::string& extension,
                                        const std::string& assetType)
    {
        AssetInfoMap& assetInfoMap = AssetsStorage.AssetInfoMap;
        auto assetInfoIter = assetInfoMap.find(extension);

        if (assetInfoIter == assetInfoMap.end())
        {
            return false;
        }

        AssetInfo& assetInfo = AssetsStorage.AssetInfoList[assetInfoIter->second];
        return assetInfo.Type == assetType;
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

    Mesh* Assets::GetMesh(AssetID meshAssetID)
    {
        const Asset& asset = GetAsset(meshAssetID);
        VnCoreAssert(asset.State == AssetState::Loaded, "can't use an unloaded asset");

        AssetInfoList& assetInfoList = AssetsStorage.AssetInfoList;
        AssetInfo& assetInfo = assetInfoList[asset.AssetInfoIndex];
        VnCoreAssert(assetInfo.Type == "Mesh", "asset type mismatch");

        return static_cast<Mesh*>(asset.Memory);
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

        if (!IsExtensionSupported(extension))
        {
            return;
        }

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

    AssetLoadingData LoadTexture(const std::string& texturepath)
    {
        AssetLoadingData textureAsset;

        int32 width;
        int32 height;
        int32 channelCount;

        stbi_set_flip_vertically_on_load(true);
        stbi_uc* pixels = stbi_load(texturepath.c_str(),
                                    &width,
                                    &height,
                                    &channelCount,
                                    0);

        if (pixels)
        {
            PixelFormat pixelFormat;

            if (channelCount == 1)
            {
                pixelFormat = PixelFormat_R;
            }
            else if (channelCount == 3)
            {
                pixelFormat = PixelFormat_RGB;
            }
            else if (channelCount == 4)
            {
                pixelFormat = PixelFormat_RGBA;
            }

            auto texture = VnAllocateStruct(Texture);

            Renderer::API.InitTexture(texture,
                                      pixels,
                                      width,
                                      height,
                                      pixelFormat,
                                      WrapMode_Repeat,
                                      WrapMode_Repeat,
                                      FilterMode_Bilinear);

            textureAsset.Memory = texture;
            textureAsset.SizeInBytes = sizeof(Texture);
            textureAsset.TotalSizeInBytes = sizeof(Texture) + (width * height * channelCount);

            stbi_image_free(pixels);
        }

        return textureAsset;
    }

    void UnloadTexture(Asset* textureAsset)
    {
        Texture* texture = static_cast<Texture*>(textureAsset->Memory);
        Renderer::API.UninitTexture(texture);
        VnFree(texture);
    }

    AssetLoadingData LoadShader(const std::string& filePath)
    {
        AssetLoadingData shaderAsset;

        Shader* shader = VnAllocateStruct(Shader);

        Renderer::API.InitShader(shader, filePath);

        shaderAsset.Memory = shader;
        shaderAsset.SizeInBytes = sizeof(Shader);
        shaderAsset.TotalSizeInBytes = sizeof(Shader);

        return shaderAsset;
    }

    void UnloadShader(Asset* shaderAsset)
    {
        Shader* shader = static_cast<Shader*>(shaderAsset->Memory);
        Renderer::API.UninitShader(shader);
        VnFree(shader);
    }

    AssetLoadingData LoadScene(const std::string& scenepath)
    {
        AssetLoadingData sceneAsset;

        Scene* scene = new Scene;

        sceneAsset.Memory = scene;
        sceneAsset.SizeInBytes = sizeof(Scene);
        sceneAsset.TotalSizeInBytes = sizeof(Scene); // @(Harlequin): not quite right entites + components

        return sceneAsset;
    }

    void UnloadScene(Asset* sceneAsset)
    {
        Scene* scene = static_cast<Scene*>(sceneAsset->Memory);
        DestroyScene(scene);
        delete scene;
    }

    AssetLoadingData LoadMesh(const std::string& meshPath)
    {
        AssetLoadingData meshAsset;

        Mesh* mesh = new Mesh;
        LoadObjMesh(meshPath.c_str(), mesh);

        // Todo(Harlequin): Right now we are just assuming we have a position uv normal
        VertexLayout layout =
        {
            { ShaderDataType_Float3, "a_Position",     false },
            { ShaderDataType_Float2, "a_TextureCoord", false },
            { ShaderDataType_Float3, "a_Normal",       false }
        };

        Renderer::InitVertexBuffer(&mesh->VertexBuffer,
                                   mesh->Vertices.data(),
                                   sizeof(Vertex) * mesh->Vertices.size(),
                                   BufferUsage::Static);

        Renderer::SetVertexBufferLayout(&mesh->VertexBuffer, &layout);

        Renderer::InitIndexBuffer32(&mesh->IndexBuffer,
                                    mesh->Indicies.data(),
                                    sizeof(uint32) * mesh->Indicies.size(),
                                    BufferUsage::Static);

        // Todo(Harlequin): Should We Free mesh->Vertices and mesh->Indicies

        meshAsset.Memory = mesh;
        meshAsset.SizeInBytes = sizeof(Mesh);
        meshAsset.TotalSizeInBytes = mesh->Vertices.size() * sizeof(Vertex) + mesh->Indicies.size() * sizeof(uint32);

        return meshAsset;
    }

    void UnloadMesh(Asset* meshAsset)
    {
        Mesh* mesh = static_cast<Mesh*>(meshAsset->Memory);
        delete mesh;
    }

    AssetsStorage Assets::AssetsStorage;
}