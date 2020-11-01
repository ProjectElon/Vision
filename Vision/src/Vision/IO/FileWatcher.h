#pragma once

#include <functional>

namespace Vision
{
    enum class FileWatcherAction
    {
        FileAdded,
        FileRemoved,
        FileModified,
        FileRenamed,
    };

    using FileWatcherCallbackFn =
    std::function<void(std::string path, FileWatcherAction action)>;

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