#include "pch.h"
#include "Vision/Core/Base.h"

#ifdef VN_PLATFORM_WINDOWS

#include "Vision/IO/FileWatcher.h"
#include "Vision/IO/FileSystem.h"

#include <winnt.h>

#include <vector>

namespace Vision
{
    struct WatchData
    {
        OVERLAPPED             Overlapped;
        std::string            DirectoryPath;
        HANDLE                 DirectoryHandle;
        uint8                  Buffer[32 * 1024];
        bool                   Recursive;
        FileWatcherCallbackFn  CallbackFn;
    };
    static std::unordered_map<std::string, WatchData> s_History;

    static void RefreshWatcher(WatchData& data, bool subscriping = true);

    static void CALLBACK WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
    {
        static bool ignoreSecondModifedAction = false; // Hack Thanks Windows
        static std::string oldName;

        if (dwNumberOfBytesTransfered == 0)
        {
            return;
        }

        if (dwErrorCode == ERROR_SUCCESS)
        {
            PFILE_NOTIFY_INFORMATION notify;
            uint64 offset = 0;
            WatchData& data = *(WatchData*)(lpOverlapped);

            do
            {
                notify = (PFILE_NOTIFY_INFORMATION)&data.Buffer[offset];
                offset += notify->NextEntryOffset;

                std::string fileName(notify->FileName, notify->FileName + (notify->FileNameLength / sizeof(WCHAR)));
                std::string path = data.DirectoryPath + "/" + fileName;
                std::replace(path.begin(), path.end(), '\\', '/');

                if (notify->Action == FILE_ACTION_ADDED)
                {
                    data.CallbackFn(path, FileWatcherAction::FileAdded);
                }
                else if (notify->Action == FILE_ACTION_REMOVED)
                {
                    data.CallbackFn(path, FileWatcherAction::FileRemoved);
                }
                if (notify->Action == FILE_ACTION_MODIFIED && !ignoreSecondModifedAction)
                {
                    data.CallbackFn(path, FileWatcherAction::FileModified);
                    ignoreSecondModifedAction = true;
                }
                else if (notify->Action == FILE_ACTION_MODIFIED && ignoreSecondModifedAction)
                {
                    ignoreSecondModifedAction = false;
                }
                else if (notify->Action == FILE_ACTION_RENAMED_OLD_NAME)
                {
                    oldName = path;
                }
                else if (notify->Action == FILE_ACTION_RENAMED_NEW_NAME)
                {
                    std::string result = oldName + "-" + path;
                    data.CallbackFn(result, FileWatcherAction::FileRenamed);
                }
            }
            while (notify->NextEntryOffset != 0);

            RefreshWatcher(data);
        }
    }

    std::string GetAbsultePathFromRelative(const std::string& path)
    {
        std::string result = path;

        if (FileSystem::FileExists(path))
        {
            result = FileSystem::GetPath(path);
        }

        if (FileSystem::IsRelativePath(path))
        {
            std::string cwd = FileSystem::GetCurrentWorkingDirectory();
            return cwd + ((path.empty()) ? "" : "/" + result);
        }

        return result;
    }

    void RefreshWatcher(WatchData& data, bool subscribing)
    {
        ReadDirectoryChangesW(
        data.DirectoryHandle,
        data.Buffer,
        sizeof(data.Buffer),
        data.Recursive,
        FILE_NOTIFY_CHANGE_LAST_WRITE |
        FILE_NOTIFY_CHANGE_CREATION |
        FILE_NOTIFY_CHANGE_SECURITY |
        FILE_NOTIFY_CHANGE_DIR_NAME |
        FILE_NOTIFY_CHANGE_FILE_NAME,
        0,
        &data.Overlapped,
        (subscribing) ? WatchCallback : 0);
    }

    FileWatcher::~FileWatcher()
    {
        for (auto& [filepath, data] : s_History)
        {
            CloseHandle(data.Overlapped.hEvent);
            CloseHandle(data.DirectoryHandle);
        }
    }

    bool FileWatcher::Watch(const std::string& path, bool recursive)
    {
        std::string watchPath = GetAbsultePathFromRelative(path);
        auto watcherIter = s_History.find(watchPath);

        if (watcherIter != s_History.end())
        {
            VN_CORE_INFO("Already Watching Directory: {0}", watchPath);
            return false;
        }

        HANDLE handle = CreateFileA(
                        watchPath.c_str(),
                        FILE_LIST_DIRECTORY,
                        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                        0,
                        OPEN_EXISTING,
                        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
                        0);

        if (handle != INVALID_HANDLE_VALUE)
        {
            WatchData& data = s_History[watchPath];
            data.DirectoryPath = watchPath;
            data.DirectoryHandle = handle;
            data.Overlapped = { 0 };
            data.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            data.CallbackFn = m_CallbackFn;
            data.Recursive = recursive;

            RefreshWatcher(data);

            VN_CORE_INFO("Watching Directory: {0}", watchPath);
            return true;
        }

        VN_CORE_INFO("Can't Watch Directory: {0}", watchPath);

        return false;
    }

    bool FileWatcher::UnWatch(const std::string& path)
    {
        std::string watchPath = GetAbsultePathFromRelative(path);

        auto watcherIter = s_History.find(watchPath);

        if (watcherIter != s_History.end())
        {
            WatchData& data = watcherIter->second;

            CancelIo(data.DirectoryHandle);
            RefreshWatcher(data, false);
            CloseHandle(data.Overlapped.hEvent);
            CloseHandle(data.DirectoryHandle);

            s_History.extract(watcherIter);

            VN_CORE_INFO("UnWatching Directory: {0}", watchPath);
            return true;
        }

        VN_CORE_INFO("Can't UnWatch Directory: {0}", watchPath);
        return false;
    }
}

#endif