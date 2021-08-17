#include "Common.h"
#include <fstream>

std::string GetFileName(const std::string& filepath, bool includeExtension)
{
    int dotIndex = filepath.rfind(".");

    // not a path to a file
    if (dotIndex == -1)
    {
        return std::string();
    }

    int lastSlash = filepath.find_last_of("/\\");

    int start = lastSlash + 1;
    int count = filepath.length() - lastSlash + 1;

    if (!includeExtension)
    {
        count = dotIndex - lastSlash - 1;
    }

    return filepath.substr(start, count);
}

std::vector<std::string> GetLines(const std::string& filepath)
{
    std::vector<std::string> result;
    std::ifstream ifs(filepath);

    if (ifs.is_open())
    {
        std::string line;

        while (std::getline(ifs, line))
        {
            result.push_back(line);
        }

        ifs.close();
    }
    else
    {
        printf("meta.txt is required\n");
    }

    return result;
}