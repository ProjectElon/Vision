#pragma once

#include <string>

namespace Vision
{
    class FileDialog
    {
    public:
        static std::string OpenFile(const std::string& filter, const std::vector<std::string>& extensions);
        static std::string SaveFile(const std::string& filter, const std::string& extension);
    };
}