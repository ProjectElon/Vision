#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

#include "Common.h"
#include "Lexer.h"
#include "Introspection.h"

int main(int argCount, char* args[])
{
    std::string metaOutputPath;

    if (argCount >= 2)
    {
        metaOutputPath = std::string(args[1]);

        if (metaOutputPath.back() != '\\' && metaOutputPath.back() != '/')
        {
            metaOutputPath += '/';
        }
    }
    else
    {
        metaOutputPath = "Meta/";
    }

    Introspection introspection;

    // @Robustness: trim lines
    std::vector<std::string> metaFilesPath = GetLines("Meta.txt");

    for (auto& filePath : metaFilesPath)
    {
        ParseAndGenerateMetaFile(&introspection, filePath, metaOutputPath);
    }

    GenerateTypeInfoMetaFile(&introspection, metaOutputPath, metaFilesPath);

    return 0;
}