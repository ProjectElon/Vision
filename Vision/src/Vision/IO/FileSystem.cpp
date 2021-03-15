#include "pch.h"
#include "Vision/Core/Common.h"
#include "Vision/IO/FileSystem.h"

namespace Vision
{
    std::string FileSystem::GetPath(const std::string& filepath)
    {
        int32 dotIndex = filepath.rfind(".");

        if (dotIndex == -1)
        {
            return filepath;
        }

        int32 lastSlash = filepath.find_last_of("/\\");

        return filepath.substr(0, lastSlash);
    }

    std::string FileSystem::GetFileName(const std::string& filepath, bool includeExtension)
    {
        int32 dotIndex = filepath.rfind(".");

        // not a path to a file
        if (dotIndex == -1)
        {
            return std::string();
        }

        int32 lastSlash = filepath.find_last_of("/\\");

        int32 start = lastSlash + 1;
        int32 count = filepath.length() - lastSlash + 1;

        if (!includeExtension)
        {
            count = dotIndex - lastSlash - 1;
        }

        return filepath.substr(start, count);
    }

    std::string FileSystem::GetFileExtension(const std::string& filepath, bool includeDot)
    {
        int32 dotIndex = filepath.rfind(".");

        // not a path to a file
        if (dotIndex == -1)
        {
            return std::string();
        }

        if (!includeDot)
        {
            dotIndex++;
        }

        return filepath.substr(dotIndex);
    }

    bool FileSystem::IsAbsolutePath(const std::string& path)
    {
        if (path.empty())
        {
            return false;
        }

        // @Note: check if this supports mobile platforms as well
        // unix
        if (path[0] == '/')
        {
            return true;
        }

        // windows
        return path.length() > 2 && isalpha(path[0]) && path[1] == ':' && (path[2] == '\\' || path[2] == '/');
    }

    bool FileSystem::IsRelativePath(const std::string& path)
    {
        return !IsAbsolutePath(path);
    }
}