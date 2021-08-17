#pragma once

#include <cstdio>
#include <string>
#include <vector>

#include "Common.h"
#include "Lexer.h"

struct InspectableParams
{
    std::string Category      = "none";
    bool HandleTypeSwitchCase = true;
};

struct StructInfo
{
    std::string       Name;
    InspectableParams Params;
};

enum TypeInfoFlags : uint32_t
{
    TypeInfo_IsConst     = BitU32(0),
    TypeInfo_IsStatic    = BitU32(1),
    TypeInfo_IsUnsigned  = BitU32(2),
    TypeInfo_IsBasicType = BitU32(3),
    TypeInfo_IsPointer   = BitU32(4),
    TypeInfo_IsArray     = BitU32(5)
};

struct Introspection
{
    std::vector<StructInfo> Structs;

    std::string CurrentNamespace = "";
    FILE* CurrentMetaFileHandle = nullptr;
};

bool IsTokenBasicType(Token* token);

void ParseMember(Introspection* introspection,
                 Lexer* lexer,
                 Token* structName,
                 Token* firstToken);

void ParseStruct(Introspection* introspection,
                 Lexer* lexer,
                 InspectableParams* params);

InspectableParams ParseInspectableParams(Introspection* introspection,
                                         Lexer* lexer);

void ParseInspectable(Introspection* introspection,
                      Lexer* lexer);

void ParseAndGenerateMetaFile(Introspection* introspection,
                              const std::string& filePath,
                              const std::string& outputFilePath);

void GenerateTypeInfoMetaFile(Introspection* introspection,
                              const std::string& outputFilePath,
                              std::vector<std::string>& filesPath);