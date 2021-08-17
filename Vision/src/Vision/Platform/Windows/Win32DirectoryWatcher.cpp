#include "pch.hpp"
#include "Vision/Core/Defines.h"
#include "Vision/Core/Log.h"

#ifdef VN_PLATFORM_WINDOWS

#include "Vision/IO/DirectoryWatcher.h"
#include "Vision/IO/FileSystem.h"

#include <windows.h>
#include <winnt.h>
#include <vector>

namespace Vision
{
    struct Win32WatchData
    {
        OVERLAPPED Overlapped;
        std::string DirectoryPath;
        HANDLE DirectoryHandle;
        uint8 Buffer[64 * 1024];
        bool Recursive;
        DirectoryWatcherCallbackFn CallbackFn;
    };

    static std::unordered_map<std::string, Win32WatchData> InternalHistory;

    static void RefreshWatcherDirectory(Win32WatchData& data, bool subscriping = true);

    static void CALLBACK WatchCallback(DWORD dwErrorCode, DWORD dwNumberOfBytesTransfered, LPOVERLAPPED lpOverlapped)
    {
        static bool ignoreFirstModifedAction = true; // @Hack: Thanks Windows
        static std::string oldPath;

        if (dwNumberOfBytesTransfered == 0)
        {
            return;
        }

        if (dwErrorCode == ERROR_SUCCESS)
        {
            PFILE_NOTIFY_INFORMATION notify;
            uint64 offset = 0;
            Win32WatchData& data = *(Win32WatchData*)(lpOverlapped);

            do
            {
                notify = (PFILE_NOTIFY_INFORMATION)&data.Buffer[offset];
                offset += notify->NextEntryOffset;

                std::string fileName(notify->FileName, notify->FileName + (notify->FileNameLength / sizeof(WCHAR)));
                std::string path = data.DirectoryPath + "/" + fileName;
                std::replace(path.begin(), path.end(), '\\', '/');

                if (notify->Action == FILE_ACTION_ADDED)
                {
                    data.CallbackFn(DirectoryWatcherAction::FileAdded, path, path);
                }
                else if (notify->Action == FILE_ACTION_REMOVED)
                {
                    data.CallbackFn(DirectoryWatcherAction::FileRemoved, path, path);
                }
                if (notify->Action == FILE_ACTION_MODIFIED && ignoreFirstModifedAction)
                {
                    ignoreFirstModifedAction = false;
                }
                else if (notify->Action == FILE_ACTION_MODIFIED && !ignoreFirstModifedAction)
                {
                    data.CallbackFn(DirectoryWatcherAction::FileModified, path, path);
                    ignoreFirstModifedAction = true;
                }
                else if (notify->Action == FILE_ACTION_RENAMED_OLD_NAME)
                {
                    oldPath = path;
                }
                else if (notify->Action == FILE_ACTION_RENAMED_NEW_NAME)
                {
                    data.CallbackFn(DirectoryWatcherAction::FileRenamed, path, oldPath);
                }
            }
            while (notify->NextEntryOffset != 0);

            RefreshWatcherDirectory(data);
        }
    }

    //@(Note): Move to file io
    std::string GetAbsultePathFromRelative(const std::string& path)
    {
        std::string result = path;

        bool isFile = FileSystem::FileExists(path);

        if (isFile)
        {
            result = FileSystem::GetFilePath(path);
        }

        if (FileSystem::IsRelativePath(path))
        {
            std::string cwd = FileSystem::GetCurrentWorkingDirectory();
            return cwd + ((path.empty()) ? "" : "/" + result);
        }

        return result;
    }

    void RefreshWatcherDirectory(Win32WatchData& data, bool subscribing)
    {
        ReadDirectoryChangesW(data.DirectoryHandle,
                              data.Buffer,
                              sizeof(data.Buffer),
                              data.Recursive,
                              FILE_NOTIFY_CHANGE_LAST_WRITE |
                              FILE_NOTIFY_CHANGE_LAST_ACCESS |
                              FILE_NOTIFY_CHANGE_CREATION |
                              FILE_NOTIFY_CHANGE_SECURITY |
                              FILE_NOTIFY_CHANGE_DIR_NAME |
                              FILE_NOTIFY_CHANGE_FILE_NAME,
                              0,
                              &data.Overlapped,
                              (subscribing) ? WatchCallback : 0);
    }

    bool WatchDirectory(const std::string& path,
                        DirectoryWatcherCallbackFn callback,
                        bool recursive)
    {
        std::string watchPath = GetAbsultePathFromRelative(path);
        auto watcherIter = InternalHistory.find(watchPath);

        if (watcherIter != InternalHistory.end())
        {
            VnCoreInfo("Already Watching Directory: {0}", watchPath);
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
            Win32WatchData& data   = InternalHistory[watchPath];
            data.DirectoryPath     = watchPath;
            data.DirectoryHandle   = handle;
            data.Overlapped        = { 0 };
            data.Overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
            data.CallbackFn        = callback;
            data.Recursive         = recursive;

            RefreshWatcherDirectory(data);

            VnCoreInfo("Watching Directory: {0}", watchPath);
            return true;
        }

        VnCoreInfo("Can't Watch Directory: {0}", watchPath);

        return false;
    }

    bool UnWatchDirectory(const std::string& path)
    {
        std::string watchPath = GetAbsultePathFromRelative(path);

        auto watcherIter = InternalHistory.find(watchPath);

        if (watcherIter != InternalHistory.end())
        {
            Win32WatchData& data = watcherIter->second;

            CancelIo(data.DirectoryHandle);
            RefreshWatcherDirectory(data, false);

            CloseHandle(data.Overlapped.hEvent);
            CloseHandle(data.DirectoryHandle);

            InternalHistory.extract(watcherIter);

            VnCoreInfo("UnWatching Directory: {0}", watchPath);
            return true;
        }

        VnCoreInfo("Can't UnWatch Directory: {0}", watchPath);
        return false;
    }
}

#endif