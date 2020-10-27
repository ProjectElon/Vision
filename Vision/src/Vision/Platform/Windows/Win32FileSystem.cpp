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
}

#endif