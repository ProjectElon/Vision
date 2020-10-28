#include "pch.h"

#include "Vision/Core/Base.h"
#ifdef VN_PLATFORM_WINDOWS
#include "Vision/IO/FileSystem.h"
#include <windows.h>

namespace Vision
{
    bool FileSystem::FileExists(const std::string& filepath)
    {
        DWORD attributes = GetFileAttributesA(filepath.c_str());
        return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
    }

    bool FileSystem::DirectoryExists(const std::string& path)
    {
        DWORD attributes = GetFileAttributesA(path.c_str());
        return (attributes != INVALID_FILE_ATTRIBUTES && (attributes & FILE_ATTRIBUTE_DIRECTORY));
    }

    std::string FileSystem::GetExecutableAbsolutePath()
    {
        char fileName[MAX_PATH];
        GetModuleFileNameA(NULL, fileName, MAX_PATH);
        return std::string(fileName);
    }

    std::string FileSystem::GetCurrentWorkingDirectory()
    {
        TCHAR path[MAX_PATH];
        DWORD length = GetCurrentDirectory(MAX_PATH, path);
        return std::string(path, path + length);
    }

    bool FileSystem::SetCurrentWorkingDirectory(const std::string& path)
    {
        std::wstring wpath(path.begin(), path.end());
        return SetCurrentDirectory(wpath.c_str());
    }

    bool FileSystem::CreateDirectory(const std::string& path)
    {
        bool success = CreateDirectoryA(path.c_str(), 0);

        if (!success)
        {
            DWORD errorCode = GetLastError();

            if (errorCode == ERROR_ALREADY_EXISTS)
            {
                VN_CORE_INFO("Directory: {0} Already Exists", path);
            }

            if (errorCode == ERROR_PATH_NOT_FOUND)
            {
                VN_CORE_INFO("Failed to create directory: {0} one or more intermediate directories do not exist", path);
            }
        }

        return success;
    }

    bool FileSystem::CreateDirectoryRecursive(const std::string& path)
    {
        uint32 firstSlash = path.find_first_of("/\\");

        uint32 count = path.size();
        uint32 index = firstSlash + 1;

        while (index < count)
        {
            if (path[index] == '/' || path[index] == '\\')
            {
                uint32 countFromStart = index;
                std::string subPath = path.substr(0, countFromStart);
                CreateDirectory(subPath);
            }

            ++index;
        }

        return CreateDirectory(path);
    }

    std::vector<std::string> FileSystem::ScanDirectory(const std::string& path, const std::vector<std::string>& extensions, bool recursive)
    {
        // @Note: CleanUp Please

        std::wstring wPath(path.begin(), path.end());

        WIN32_FIND_DATAW info;
        HANDLE handle = FindFirstFileW((wPath + L"/*").c_str(), &info);

        if (handle == INVALID_HANDLE_VALUE)
        {
            return std::vector<std::string>();
        }

        std::vector<std::string> result;

        struct Directory
        {
            std::wstring Path;
            HANDLE Handle;
        };

        std::stack<Directory> directories;
        directories.push({ wPath, handle });

        while (!directories.empty())
        {
            const Directory& currentDirectory = directories.top();

            std::wstring wCurrentPath = currentDirectory.Path;
            std::wstring wName = info.cFileName;

            if (!wName.empty())
            {
                // @Note: Should We Include Hidden Files
                // info.dwFileAttributes & FILE_ATTRIBUTE_HIDDEN

                if (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
                {
                    if (recursive && wName != L"." && wName != L"..")
                    {
                        std::wstring wNextPath = wCurrentPath + L"/" + wName;
                        directories.push({ wNextPath, FindFirstFileW((wNextPath + L"/*").c_str(), &info) });
                    }
                }
                else
                {
                    std::string name(wName.begin(), wName.end());
                    std::string extension = FileSystem::GetFileExtension(name, false);

                    if (extensions.empty() || std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
                    {
                        std::wstring wFilePath = wCurrentPath + L"/" + wName;
                        std::string filePath(wFilePath.begin(), wFilePath.end());

                        result.push_back(filePath);
                    }
                }
            }

            if (!FindNextFileW(currentDirectory.Handle, &info))
            {
                directories.pop();
            }
        }

        FindClose(handle);

        return result;
    }
}

#endif