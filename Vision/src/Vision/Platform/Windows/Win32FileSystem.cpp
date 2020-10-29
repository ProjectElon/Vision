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

        std::vector<std::string> result;

        while (FindNextFileA(handle, &info))
        {
            bool isDirectory = info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY;

            if (isDirectory)
            {
                continue;
            }

            std::string extension = FileSystem::GetFileExtension(info.cFileName, false);

            if (extensions.empty() || std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
            {
                std::string filepath = path + "/" + info.cFileName;
                result.push_back(filepath);
            }
        }

        FindClose(handle);

        return result;
    }

    struct DirectoryInfo
    {
        HANDLE           Handle;
        WIN32_FIND_DATAA Info;
        std::string      Path;
    };

    std::vector<std::string> FileSystem::ScanDirectoryRecursive(const std::string& path, const std::vector<std::string>& extensions)
    {
        DirectoryInfo parentDirectory;
        parentDirectory.Handle = FindFirstFileA((path + "/*").c_str(), &parentDirectory.Info);
        parentDirectory.Path = path;

        if (parentDirectory.Handle == INVALID_HANDLE_VALUE)
        {
            return std::vector<std::string>();
        }

        std::stack<DirectoryInfo> directories;        
        directories.push(parentDirectory);

        std::vector<std::string> result;

        while (!directories.empty())
        {
            DirectoryInfo& currentDirectory = directories.top();

            if (!FindNextFileA(currentDirectory.Handle, &currentDirectory.Info))
            {
                directories.pop();
                continue;
            }

            std::string name = currentDirectory.Info.cFileName;
            bool isDirectory = (currentDirectory.Info.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY);

            if (isDirectory)
            {
                if (name != "." && name != "..")
                {
                    DirectoryInfo nextDirectory;
                    nextDirectory.Path = currentDirectory.Path + "/" + name;
                    nextDirectory.Handle = FindFirstFileA((nextDirectory.Path + "/*").c_str(), &nextDirectory.Info);
                    directories.push(nextDirectory);
                }
            }
            else
            {
                std::string extension = FileSystem::GetFileExtension(name, false);

                if (extensions.empty() || std::find(extensions.begin(), extensions.end(), extension) != extensions.end())
                {
                    std::string currentFilePath = currentDirectory.Path + "/" + name;
                    result.push_back(currentFilePath);
                }
            }
        }

        FindClose(parentDirectory.Handle);

        return result;
    }
}

#endif