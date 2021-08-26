#include "ContentBrowserPanel.h"

#include "Vision/Core/Defines.h"
#include "Vision/Core/Logger.h"
#include "Vision/Renderer/Renderer.h"

#include "Vision/Platform/FileSystem.h"

#include <imgui.h>

#include <string>

namespace Vision
{
    // TODO(Harlequin): maybe instead of loading all the assets of a directory
    // we can query the operation system for the thumbnails
    // and for custom assets we can use an icon texture with the asset name
    static const std::string AssetsDirectory = "Assets";

    static void ReleaseCurrentDirectoryResources(ContentBrowserPanel* contentBrowserPanel);

    ContentBrowserPanel::ContentBrowserPanel()
        : CurrentDirectory(AssetsDirectory)
        , CurrentPathIndex(-1)
    {
        DirectoryIcon = Assets::RequestAsset("Resources/Icons/DirectoryIcon.png");
        FileIcon      = Assets::RequestAsset("Resources/Icons/FileIcon.png");
        BackIcon      = Assets::RequestAsset("Resources/Icons/Back.png");
        ForwardIcon   = Assets::RequestAsset("Resources/Icons/Forward.png");

        WatchDirectory(CurrentDirectory, VnBindWatcherFn(ContentBrowserPanel::OnDirectoryWatchCallback), false);
        CurrentDirectoryContents = FileSystem::ScanDirectory(CurrentDirectory, {}, true);
    }

    void ContentBrowserPanel::OnImGuiRender()
    {
        ImGui::Begin("Content Browser");
        
        IsInteractable = ImGui::IsWindowFocused() && ImGui::IsWindowHovered();
        
        bool isBackIconEnabled = CurrentDirectory != AssetsDirectory;

        auto backIconTexture = Assets::GetTexture(BackIcon);

        if (!isBackIconEnabled) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

        if (ImGui::ImageButton(*(ImTextureID*)Renderer::ConvertTextureToImGuiTexture(backIconTexture),
                                { 28, 28 },
                                { 0, 1 },
                                { 1, 0 }) && isBackIconEnabled)
        {
            UnWatchDirectory(CurrentDirectory);
            CurrentDirectory = FileSystem::GetParentPath(CurrentDirectory);
            WatchDirectory(CurrentDirectory, VnBindWatcherFn(ContentBrowserPanel::OnDirectoryWatchCallback), false);

            CurrentDirectoryContents = FileSystem::ScanDirectory(CurrentDirectory, {}, true);

            --CurrentPathIndex;

            ReleaseCurrentDirectoryResources(this);
        }

        if (!isBackIconEnabled) ImGui::PopStyleColor();

        ImGui::SameLine();

        bool isForwardIconEnabled = CurrentPathIndex + 1 < PathHistory.size();

        auto forwardIconTexture = Assets::GetTexture(ForwardIcon);
        
        if (!isForwardIconEnabled) ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));

        if (ImGui::ImageButton(*(ImTextureID*)Renderer::ConvertTextureToImGuiTexture(forwardIconTexture),
                                { 28, 28 },
                                { 0, 1 },
                                { 1, 0 }) && isForwardIconEnabled)
        {
            UnWatchDirectory(CurrentDirectory);
            CurrentDirectory = CurrentDirectory + "/" + PathHistory[CurrentPathIndex + 1];
            WatchDirectory(CurrentDirectory, VnBindWatcherFn(ContentBrowserPanel::OnDirectoryWatchCallback), false);

            CurrentDirectoryContents = FileSystem::ScanDirectory(CurrentDirectory, {}, true);
            ++CurrentPathIndex;

            ReleaseCurrentDirectoryResources(this);
        }

        if (!isForwardIconEnabled) ImGui::PopStyleColor();

        ImGui::SameLine();

        ImGui::TextWrapped(CurrentDirectory.c_str());

        static float32 padding = 16.0f;
        static float32 thumbnailSize = 72.0f;

        float32 cellSize   = thumbnailSize + padding;
        float32 panelWidth = ImGui::GetContentRegionAvail().x;
        uint32 columnCount = static_cast<uint32>(panelWidth / cellSize);

        if (columnCount < 1)
        {
            columnCount = 1;
        }

        ImGui::Columns(columnCount, 0, false);
        
        uint32 itemID = 0;

        for (uint32 i = 0; i < CurrentDirectoryContents.size(); i++)
        {
            std::string directoryItem = CurrentDirectoryContents[i];

            itemID++;
            ImGui::PushID(itemID);

            std::string extension = FileSystem::GetFileExtension(directoryItem, false);

            if (extension == "ini" ||
                extension == "vs"  ||
                extension == "git")
            {
                ImGui::PopID();
                continue;
            }

            std::string itemPath = CurrentDirectory + "/" + directoryItem;
            bool isDirectory = FileSystem::DirectoryExists(itemPath);

            std::string itemName = "";

            if (isDirectory)
            {
                itemName = FileSystem::GetDirectoryName(itemPath);
            }
            else
            {
                itemName = FileSystem::GetFileName(itemPath, true);
            }

            Texture* itemIconTexture = nullptr;

            if (isDirectory)
            {
                itemIconTexture = Assets::GetTexture(DirectoryIcon);
            }
            else if (Assets::IsExtensionOfAssetType(extension, "Texture"))
            {
                auto assetIter = CurrentDirectoryAssets.find(itemName);
                
                AssetID iconTextureID = 0;
                
                if (assetIter == CurrentDirectoryAssets.end())
                {
                    iconTextureID = Assets::RequestAsset(directoryItem);
                    CurrentDirectoryAssets.emplace(itemName, iconTextureID);
                }
                else
                {
                    iconTextureID = assetIter->second;
                }
                
                itemIconTexture = Assets::GetTexture(iconTextureID);
            }
            else
            {
                itemIconTexture = Assets::GetTexture(FileIcon);
            }

            ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0, 0, 0, 0));
           

            if (ImGui::ImageButton(*(ImTextureID*)Renderer::ConvertTextureToImGuiTexture(itemIconTexture),
                                   { thumbnailSize, thumbnailSize },
                                   { 0, 1 },
                                   { 1, 0 }))
            {
                if (isDirectory)
                {
                    ReleaseCurrentDirectoryResources(this);

                    if (CurrentPathIndex == (int32)PathHistory.size() - 1)
                    {
                        PathHistory.push_back(itemName);
                        ++CurrentPathIndex;
                    }
                    else
                    {
                        if (CurrentPathIndex + 1 < (int32)PathHistory.size())
                        {
                            if (PathHistory[CurrentPathIndex + 1] == itemName)
                            {
                                ++CurrentPathIndex;
                            }
                            else
                            {
                                PathHistory.resize(CurrentPathIndex + 1);
                                PathHistory.push_back(itemName);
                                ++CurrentPathIndex;
                            }
                        }
                    }
                }
            }

            ImGui::PopStyleColor();

            if (ImGui::BeginDragDropSource(ImGuiDragDropFlags_None))
            {
                std::string pathString = directoryItem;
                
                ImGui::SetDragDropPayload("Content_Browser_Item",
                                          pathString.data(),
                                          pathString.size() + 1,
                                          ImGuiCond_Once);
                
                ImGui::EndDragDropSource();
            }

            if (ImGui::IsItemHovered() && ImGui::IsMouseDoubleClicked(ImGuiMouseButton_Left))
            {
                if (isDirectory)
                {
                    UnWatchDirectory(CurrentDirectory);
                    CurrentDirectory = CurrentDirectory + "/" + itemName;
                    WatchDirectory(CurrentDirectory, VnBindWatcherFn(ContentBrowserPanel::OnDirectoryWatchCallback), false);

                    CurrentDirectoryContents = FileSystem::ScanDirectory(CurrentDirectory, {}, true);
                }
            }

            ImGui::TextWrapped(itemName.c_str());
            ImGui::NextColumn();

            ImGui::PopID();
        }

        ImGui::Columns(1);
#if 0
        ImGui::SliderFloat("Thumbnail", &thumbnailSize, 1, 256);
        ImGui::SliderFloat("Padding", &padding, 0, 32);
#endif
        ImGui::End();
    }
    
    void ContentBrowserPanel::OnDirectoryWatchCallback(DirectoryWatcherAction action,
                                                       std::string path,
                                                       std::string oldPath)
    {
        switch (action)
        {
            case DirectoryWatcherAction::FileAdded:
            case DirectoryWatcherAction::FileModified:
            case DirectoryWatcherAction::FileRenamed:
            case DirectoryWatcherAction::FileRemoved:
            {
                CurrentDirectoryContents = FileSystem::ScanDirectory(CurrentDirectory, {}, true);
            } break;
        }
    }

    void ReleaseCurrentDirectoryResources(ContentBrowserPanel* contentBrowserPanel)
    {
        if (contentBrowserPanel->CurrentDirectoryAssets.empty())
        {
            for (auto& [filename, assetID] : contentBrowserPanel->CurrentDirectoryAssets)
            {
                Assets::ReleaseAsset(assetID);
            }

            contentBrowserPanel->CurrentDirectoryAssets.clear();
        }
    }
}