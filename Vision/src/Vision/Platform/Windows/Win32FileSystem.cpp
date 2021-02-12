#include "pch.h"

#include "Vision/Core/Common.h"
#ifdef VN_PLATFORM_WINDOWS
#include "Vision/IO/FileSystem.h"
#include <fileapi.h>

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
        char path[MAX_PATH];
        GetModuleFileNameA(0, path, ARRAYSIZE(path));

        std::string result = path;
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }

    std::string FileSystem::GetCurrentWorkingDirectory()
    {
        char path[MAX_PATH];
        GetCurrentDirectoryA(ARRAYSIZE(path), path);
        std::string result = path;
        std::replace(result.begin(), result.end(), '\\', '/');
        return result;
    }

    bool FileSystem::SetCurrentWorkingDirectory(const std::string& path)
    {
        return SetCurrentDirectoryA(path.c_str());
    }

    bool FileSystem::CreateDirectory(const std::string& path)
    {
        return CreateDirectoryA(path.c_str(), 0);
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

    std::vector<std::string> FileSystem::ScanDirectory(const std::string& path, const std::vector<std::string>& extensions)
    {
        WIN32_FIND_DATAA info;
        HANDLE handle = FindFirstFileA((path + "/*").c_str(), &info);

        if (handle == INVALID_HANDLE_VALUE)
        {
            return std::vector<std::string>();
        }

        // @Note: so we can make the . in the extensions optional
        bool includeDot = true;

        if (!extensions.empty() && extensions.front()[0] != '.')
        {
            includeDot = false;
        }

        std::vector<std::string> result;

        while (FindNextFileA(handle, &info))
        {
            bool isDirectory = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

            if (isDirectory)
            {
                continue;
            }

            std::string name = info.cFileName;

            std::string extension = FileSystem::GetFileExtension(name, includeDot);

            if (extensions.empty() || std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
            {
                std::string filepath = path + "/" + name;
                std::replace(filepath.begin(), filepath.end(), '\\', '/');
                result.push_back(filepath);
            }
        }

        FindClose(handle);

        return result;
    }

    struct DirectoryInfo
    {
        HANDLE      Handle;
        std::string Path;
    };

    std::vector<std::string> FileSystem::ScanDirectoryRecursive(const std::string& path, const std::vector<std::string>& extensions)
    {
        WIN32_FIND_DATAA info;

        DirectoryInfo parentDirectory;
        parentDirectory.Handle = FindFirstFileA((path + "/*").c_str(), &info);
        parentDirectory.Path = path;

        if (parentDirectory.Handle == INVALID_HANDLE_VALUE)
        {
            return std::vector<std::string>();
        }

        // @Note: so we can make the . in the extensions optional
        bool includeDot = true;

        if (!extensions.empty() && extensions.front()[0] != '.')
        {
            includeDot = false;
        }

        std::stack<DirectoryInfo> directories;
        directories.push(parentDirectory);

        std::vector<std::string> result;

        while (!directories.empty())
        {
            const DirectoryInfo& currentDirectory = directories.top();

            if (!FindNextFileA(currentDirectory.Handle, &info))
            {
                directories.pop();
                continue;
            }

            std::string name = info.cFileName;
            bool isDirectory = (info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

            if (isDirectory)
            {
                if (name != "." && name != "..")
                {
                    DirectoryInfo nextDirectory;

                    nextDirectory.Path = currentDirectory.Path + "/" + name;
                    nextDirectory.Handle = FindFirstFileA((nextDirectory.Path + "/*").c_str(), &info);

                    directories.push(nextDirectory);
                }
            }
            else
            {
                std::string extension = FileSystem::GetFileExtension(name, includeDot);

                if (extensions.empty() || std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
                {
                    std::string currentFilePath = currentDirectory.Path + "/" + name;

                    std::replace(currentFilePath.begin(), currentFilePath.end(), '\\', '/');
                    result.push_back(currentFilePath);
                }
            }
        }

        FindClose(parentDirectory.Handle);

        return result;
    }
}

#endif