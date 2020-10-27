#include "pch.h"
#include "Vision/Core/Base.h"
#include "Vision/IO/FileSystem.h"

namespace Vision
{
    std::string FileSystem::GetFileName(const std::string& filepath, bool includeExtension)
    {
        uint32 dotIndex = filepath.rfind(".");

        // not a path to a file
        if (dotIndex == -1)
        {
            return std::string();
        }

        uint32 lastSlash = filepath.find_last_of("/\\");

        uint32 start = lastSlash + 1;
        uint32 count = filepath.length() - lastSlash + 1;

        if (!includeExtension)
        {
            count = dotIndex - lastSlash - 1;
        }

        return filepath.substr(start, count);
    }

    std::string FileSystem::GetFileExtension(const std::string& filepath, bool includeDot)
    {
        uint32 dotIndex = filepath.rfind(".");

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

        // @Note: Double Check if this support mobile platforms

        // unix
        if (path[0] == '/')
        {
            return true;
        }

        // windows
        return path.length() > 2 && isalpha(path[0]) && path[1] == ':' && (path[2] == '\\' || path[2] == '/');
    }
}