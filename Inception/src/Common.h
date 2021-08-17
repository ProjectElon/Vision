#pragma once

#define ArrayCount(Array) sizeof(Array) / sizeof(Array[0])
#define BitU32(bitIndex) (1u << (bitIndex))

#include <vector>
#include <string>

struct String
{
    char* Data = nullptr;
    uint32_t Count = 0;
};

std::vector<std::string> GetLines(const std::string& filepath);
std::string GetFileName(const std::string& filepath, bool includeExtension);