#pragma once

#include <string>

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

        // Platform Ignostic
        static std::string GetFileName(const std::string& filepath, bool includeExtension = true);
        static std::string GetFileExtension(const std::string& filepath, bool includeDot = true);
        static bool IsAbsolutePath(const std::string& path);
    };
}