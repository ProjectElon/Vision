#pragma once

#include <functional>

namespace Vision
{
    #define VnBindWatcherFn(fn) std::bind(&fn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

    enum class FileWatcherAction
    {
        FileAdded,
        FileRemoved,
        FileModified,
        FileRenamed,
    };

    using FileWatcherCallbackFn =
    std::function<void(FileWatcherAction action, std::string path, std::string oldPath)>;

    bool WatchDirectory(const std::string& path,
                        FileWatcherCallbackFn callback,
                        bool recursive = true);

    bool UnWatchDirectory(const std::string& path);
}