#pragma once

#include <functional>

namespace Vision
{
    #define VnBindWatcherFn(fn) std::bind(&fn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

    enum class DirectoryWatcherAction
    {
        FileAdded,
        FileRemoved,
        FileModified,
        FileRenamed,
    };

    using DirectoryWatcherCallbackFn =
        std::function<void(DirectoryWatcherAction action, std::string path, std::string oldPath)>;

    bool WatchDirectory(const std::string& path,
                        DirectoryWatcherCallbackFn callback,
                        bool recursive = true);

    bool UnWatchDirectory(const std::string& path);
}