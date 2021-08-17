#pragma once


#include "Vision/IO/DirectoryWatcher.h"
#include "Vision/IO/Assets.h"

#include <vector>

namespace Vision
{
   struct ContentBrowserPanel
   {
      std::string CurrentDirectory;
      std::vector<std::string> CurrentDirectoryContents;
      std::vector<std::string> PathHistory;
      int32 CurrentPathIndex;
      std::unordered_map<std::string, AssetID> CurrentDirectoryAssets;

      AssetID ActiveSceneID = 0;
      AssetID DirectoryIcon = 0;
      AssetID FileIcon = 0;
      AssetID BackIcon = 0;
      AssetID ForwardIcon = 0;

      bool IsInteractable = false;

      ContentBrowserPanel();

      void OnImGuiRender();

      void OnDirectoryWatchCallback(DirectoryWatcherAction action,
                                    std::string path,
                                    std::string oldPath);
   };
}