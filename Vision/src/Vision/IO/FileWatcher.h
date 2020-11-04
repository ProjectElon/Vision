#pragma once

#include <functional>

namespace Vision
{
    #define BindWatcherFn(fn) std::bind(&fn, this, std::placeholders::_1, std::placeholders::_2, std::placeholders::_3)

    enum class FileWatcherAction
    {
        FileAdded,
        FileRemoved,
        FileModified,
        FileRenamed,
    };

    using FileWatcherCallbackFn =
    std::function<void(FileWatcherAction action, std::string path, std::string oldPath)>;

    class FileWatcher
    {
    public:
        FileWatcher(const FileWatcherCallbackFn& callbackFn)
            : m_CallbackFn(callbackFn)
        {
        }

        ~FileWatcher();

        bool Watch(const std::string& path, bool recursive = true);
        bool UnWatch(const std::string& path);

    private:
        FileWatcherCallbackFn m_CallbackFn;
    };
}