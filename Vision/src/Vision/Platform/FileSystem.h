#pragma once

#include <string>

#include "Vision/Core/Defines.h"

// @Note: Thanks Windows
#ifdef VN_PLATFORM_WINDOWS
#undef CreateDirectory
#endif

namespace Vision
{
    class FileSystem
    {
    public:
        // Platform Specific
        static bool FileExists(const std::string& filepath);
        static bool DirectoryExists(const std::string& path);
        
        static std::string GetExecutableAbsolutePath();

        static std::string GetCurrentWorkingDirectory();
        static bool SetCurrentWorkingDirectory(const std::string& path);

        static std::vector<std::string> ScanDirectory(const std::string& path,
                                                      const std::vector<std::string>& extensions = {},
                                                      bool includeSubDirectories = false);
        static std::vector<std::string> ScanDirectoryRecursive(const std::string& parentPath,
                                                               const std::vector<std::string>& extensions = {},
                                                               bool includeSubDirectories = false);

        static bool MakeDirectory(const std::string& path);
        static bool MakeDirectoryRecursive(const std::string& path);

        // Platform Ignostic
        static std::string GetDirectoryName(const std::string& path);
        static std::string GetFilePath(const std::string& filepath);
        static std::string GetFileName(const std::string& filepath, bool includeExtension = true);
        static std::string GetFileExtension(const std::string& filepath, bool includeDot = true);
        static bool IsAbsolutePath(const std::string& path);
        static bool IsRelativePath(const std::string& path);

        static std::string GetParentPath(const std::string& path);
    };
}