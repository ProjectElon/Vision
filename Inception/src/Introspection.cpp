#include "Introspection.h"

#include <sstream>

static const char* BasicTypes[] =
{
    "bool",
    "char",
    "s8",
    "int8",
    "int8_t",
    "u8",
    "uint8",
    "uint8_t",
    "short",
    "s16",
    "int16",
    "int16_t",
    "u16",
    "uint16",
    "uint16_t",
    "int",
    "long",
    "s32",
    "int32",
    "int32_t",
    "u32",
    "uint32",
    "uint32_t",
    "u64",
    "int64",
    "int64_t",
    "long_long",
    "uint64",
    "uint64_t",
    "float",
    "float_t",
    "float32",
    "f32",
    "float64",
    "f64",
    "double",
    "double_t",
    "size_t",
    "memorysize"
};

bool IsTokenBasicType(Token* token)
{
    for (uint32_t typeIndex = 0; typeIndex < ArrayCount(BasicTypes); ++typeIndex)
    {
        if (token->IsEquals(BasicTypes[typeIndex]))
        {
            return true;
        }
    }

    return false;
}

void ParseMember(Introspection* introspection,
                 Lexer* lexer,
                 Token* structName,
                 Token* firstToken)
{
    bool hasOpenBracket = false;
    bool hasCloseBracket = false;

    Token memberTypeToken;
    Token identifierToken;
    Token arrayCountToken;

    Token token = *firstToken;

    uint32_t longCount = 0;
    uint32_t typeInfoFlags = 0;

    bool parsing = true;

    while (parsing)
    {
        switch (token.Type)
        {
            case TokenType::Asterisk:
            {
                typeInfoFlags |= TypeInfo_IsPointer;
            }
            break;

            case TokenType::Identifier:
            {
                if (token.IsEquals("const"))
                {
                    typeInfoFlags |= TypeInfo_IsConst;
                }
                else if (token.IsEquals("static"))
                {
                    typeInfoFlags |= TypeInfo_IsStatic;
                }
                else if (token.IsEquals("unsigned"))
                {
                    typeInfoFlags |= TypeInfo_IsUnsigned;
                }
                else if (token.IsEquals("long"))
                {
                    longCount++;
                    typeInfoFlags |= TypeInfo_IsBasicType;
                }
                else if (IsTokenBasicType(&token))
                {
                    memberTypeToken = token;
                    typeInfoFlags |= TypeInfo_IsBasicType;
                }
                else if (memberTypeToken.Text.Count == 0)
                {
                    if (longCount == 2)
                    {
                        token.Text.Data  = "long_long";
                        token.Text.Count = strlen("long_long");
                    }
                    else
                    {
                        memberTypeToken = token;
                    }
                }
                else if (identifierToken.Text.Count == 0)
                {
                    identifierToken = token;
                }
            }
            break;

            case TokenType::OpenBracket:
            {
                hasOpenBracket = true;
            }
            break;

            case TokenType::CloseBracket:
            {
                hasCloseBracket = true;
            }
            break;

            case TokenType::Integer:
            {
                if (hasOpenBracket && !hasCloseBracket)
                {
                    arrayCountToken = token;
                }
            }
            break;

            case TokenType::Semicolon:
            case TokenType::EndOfStream:
            {
                parsing = false;
            }
            break;
        }

        if (parsing)
        {
            token = lexer->GetNextToken();
        }
    }

    if (arrayCountToken.Text.Count == 0)
    {
        arrayCountToken.Text.Count = 1;
        arrayCountToken.Text.Data = (char*)"1";
    }

    if (hasOpenBracket && hasCloseBracket)
    {
        typeInfoFlags |= TypeInfo_IsArray;
    }

    std::string arrayCountStr = "0";
    std::string identifier = std::string(identifierToken.Text.Data, identifierToken.Text.Count);

    if (typeInfoFlags & TypeInfo_IsArray)
    {
        if (arrayCountToken.IsEquals(""))
        {
            arrayCountStr = "sizeof(" + identifier + ") / sizeof(" + identifier + "[0])";
        }
        else
        {
            arrayCountStr = std::string(arrayCountToken.Text.Data, arrayCountToken.Text.Count);
        }
    }

    std::string typeSizeStr = "";

    if (typeInfoFlags & TypeInfo_IsBasicType)
    {
        typeSizeStr = "sizeof(" +
                      std::string(memberTypeToken.Text.Data, memberTypeToken.Text.Count) +
                      ")";
    }
    else
    {
        typeSizeStr = "sizeof(" +
                      introspection->CurrentNamespace +
                      std::string(memberTypeToken.Text.Data, memberTypeToken.Text.Count) +
                      ")";
    }

    fprintf(introspection->CurrentMetaFileHandle,
            "\t{ \"%.*s\", MetaType_%.*s, offsetof(%.*s%.*s, %.*s), %.*s, %.*s, %u },\n",
            identifierToken.Text.Count,
            identifierToken.Text.Data,
            memberTypeToken.Text.Count,
            memberTypeToken.Text.Data,
            introspection->CurrentNamespace.length(),
            introspection->CurrentNamespace.c_str(),
            structName->Text.Count,
            structName->Text.Data,
            identifierToken.Text.Count,
            identifierToken.Text.Data,
            arrayCountStr.length(),
            arrayCountStr.c_str(),
            typeSizeStr.length(),
            typeSizeStr.c_str(),
            typeInfoFlags);
}

void ParseStruct(Introspection* introspection,
                 Lexer* lexer,
                 InspectableParams* params)
{
    Token structName = lexer->GetNextToken();

    StructInfo structInfo;
    structInfo.Name = std::string(structName.Text.Data, structName.Text.Count);
    structInfo.Params = *params;

    introspection->Structs.push_back(structInfo);

    // @Robustness: we don't handle inheritance maybe we shouldn't hehe
    while (true)
    {
        Token token = lexer->GetNextToken();

        if (token.Type == TokenType::OpenBrace)
        {
            break;
        }
    }

    fprintf(introspection->CurrentMetaFileHandle,
            "static StructMember %.*sMembers[] = \n",
            structName.Text.Count,
            structName.Text.Data);

    fprintf(introspection->CurrentMetaFileHandle, "{\n");

    while (true)
    {
        Token token = lexer->GetNextToken();

        if (token.Type == TokenType::CloseBrace || token.IsEquals("ignore_methods"))
        {
            break;
        }
        else
        {
            ParseMember(introspection, lexer, &structName, &token);
        }
    }

    fprintf(introspection->CurrentMetaFileHandle, "};\n\n");
}

InspectableParams ParseInspectableParams(Introspection* introspection,
                                         Lexer* lexer)
{
    InspectableParams params;
    params.HandleTypeSwitchCase = true;

    bool parsing = true;

    while (parsing)
    {
        Token token = lexer->GetNextToken();

        switch (token.Type)
        {
            case TokenType::String:
            {
                params.Category = std::string(token.Text.Data, token.Text.Count);
            }
            break;

            case TokenType::Bool:
            {
                if (token.IsEquals("true"))
                {
                    params.HandleTypeSwitchCase = true;
                }
                else if (token.IsEquals("false"))
                {
                    params.HandleTypeSwitchCase = false;
                }
            }
            break;

            case TokenType::CloseParen:
            case TokenType::EndOfStream:
            {
                parsing = false;
            }
            break;
        }
    }

    return params;
}

void ParseInspectable(Introspection* introspection, Lexer* lexer)
{
    Token token = lexer->GetNextToken();

    if (token.Type == TokenType::OpenParen)
    {
        InspectableParams params = ParseInspectableParams(introspection, lexer);

        token = lexer->GetNextToken();

        if (token.IsEquals("struct"))
        {
            ParseStruct(introspection, lexer, &params);
        }
        else
        {
            lexer->Input.Count += token.Text.Count;
            lexer->Input.Data = token.Text.Data;
        }
    }
    else
    {
        printf("missing parentheses");
    }
}

void ParseAndGenerateMetaFile(Introspection* introspection,
                              const std::string& filePath,
                              const std::string& outputFilePath)
{
    FILE* fileHandle = fopen(filePath.c_str(), "r");

    String input;

    if (fileHandle)
    {
        fseek(fileHandle, 0, SEEK_END);
        size_t sizeInBytes = ftell(fileHandle);
        fseek(fileHandle, 0, SEEK_SET);

        input.Count = sizeInBytes;
        input.Data  = static_cast<char*>(malloc(sizeof(char) * sizeInBytes));

        fread(input.Data, sizeInBytes, 1, fileHandle);

        fclose(fileHandle);
    }
    else
    {
        printf("unable to open file: %s", filePath.c_str());
        return;
    }

    std::string outputFileName = GetFileName(filePath, false);
    std::string metaFilePath = outputFilePath + outputFileName + "Meta.h";

    FILE* metaFileHandle = fopen(metaFilePath.c_str(), "w");

    if (!metaFileHandle)
    {
        printf("unable to open file: %s", metaFilePath.c_str());
        return;
    }

    introspection->CurrentNamespace = "";
    introspection->CurrentMetaFileHandle = metaFileHandle;

    fprintf(metaFileHandle, "#pragma once\n\n");
    fprintf(metaFileHandle, "#include \"Meta.h\"\n\n");
    fprintf(metaFileHandle, "#include \"../%s\"\n\n", filePath.c_str());

    Lexer lexer;
    lexer.Input = input;

    if (input.Count >= 1) lexer.At[0] = input.Data[0];
    if (input.Count >= 2) lexer.At[1] = input.Data[1];

    bool parsing = true;

    while (parsing)
    {
        Token token = lexer.GetNextToken();

        switch (token.Type)
        {
            case TokenType::Identifier:
            {
                if (token.IsEquals("namespace"))
                {
                    Token nextToken = lexer.GetNextToken();
                    Token nextNextToken = lexer.GetNextToken();

                    if (nextToken.Type == TokenType::Identifier, nextNextToken.Type == TokenType::OpenBrace)
                    {
                        introspection->CurrentNamespace += std::string(nextToken.Text.Data, nextToken.Text.Count) + "::";
                    }
                }

                if (token.IsEquals("introspect_struct"))
                {
                    ParseInspectable(introspection, &lexer);
                }
            }
            break;

            case TokenType::EndOfStream:
            {
                parsing = false;
            }
            break;
        }
    }

    free(input.Data);
    fclose(metaFileHandle);

    introspection->CurrentMetaFileHandle = nullptr;
}

void GenerateTypeInfoMetaFile(Introspection* introspection,
                              const std::string& outputFilePath,
                              std::vector<std::string>& metaFilesPath)
{

    std::string metaFilePath = outputFilePath + std::string("Meta.h");
    FILE* metaFileHandle = fopen(metaFilePath.c_str(), "w");

    fprintf(metaFileHandle, "#pragma once\n\n");
    fprintf(metaFileHandle, "enum MetaType\n{\n");

    for (uint32_t typeIndex = 0; typeIndex < ArrayCount(BasicTypes); ++typeIndex)
    {
        fprintf(metaFileHandle, "\tMetaType_%s,\n", BasicTypes[typeIndex]);
    }

    for (auto& structInfo : introspection->Structs)
    {
        fprintf(metaFileHandle,
                "\tMetaType_%.*s,\n",
                structInfo.Name.length(),
                structInfo.Name.c_str());
    }

    fprintf(metaFileHandle, "};\n\n");

    const char* typeInfoString = "enum TypeInfoFlags : uint32_t\n"
                                 "{\n"
                                 "\tTypeInfo_IsConst     = 1u << 0,\n"
                                 "\tTypeInfo_IsStatic    = 1u << 1,\n"
                                 "\tTypeInfo_IsUnsigned  = 1u << 2,\n"
                                 "\tTypeInfo_IsBasicType = 1u << 3,\n"
                                 "\tTypeInfo_IsPointer   = 1u << 4,\n"
                                 "\tTypeInfo_IsArray     = 1u << 5\n"
                                 "};\n";

    fprintf(metaFileHandle, "%s\n", typeInfoString);

    const char* structMemberString = "struct StructMember\n"
                                     "{\n"
                                     "\tconst char*  Name;\n"
                                     "\tMetaType     Type;\n"
                                     "\tunsigned int Offset;\n"
                                     "\tunsigned int ArrayCount;\n"
                                     "\tunsigned int TypeSize;\n"
                                     "\tunsigned int TypeInfoFlags;\n"
                                     "};\n";

    fprintf(metaFileHandle, "%s\n", structMemberString);

    for (auto& metaFilePath : metaFilesPath)
    {
        std::string metaFileName = GetFileName(metaFilePath, false);
        fprintf(metaFileHandle, "#include \"%sMeta.h\"\n", metaFileName.c_str());
    }

    fprintf(metaFileHandle, "\n#define HandleMetaTypeCases(MemberPointer) \\\n");

    const char* structCaseString =
        "\tcase MetaType_%.*s:\\\n"
        "\t{\\\n"
        "\t\tInspectStruct(%.*sMembers,\\\n"
        "                  sizeof(%.*sMembers) / sizeof(%.*sMembers[0]),\\\n"
        "                  (%.*s*)MemberPointer,\\\n"
        "                  member->Name,\\\n"
        "                  memberFn,\\\n"
        "                  arrayIndex,\\\n"
        "                  depth + 1); \\\n"
        "\t}\\\n"
        "\tbreak;\\\n";

    for (auto& structInfo : introspection->Structs)
    {
        if (structInfo.Params.HandleTypeSwitchCase)
        {
            fprintf(metaFileHandle,
                    structCaseString,
                    structInfo.Name.length(), structInfo.Name.c_str(),
                    structInfo.Name.length(), structInfo.Name.c_str(),
                    structInfo.Name.length(), structInfo.Name.c_str(),
                    structInfo.Name.length(), structInfo.Name.c_str(),
                    structInfo.Name.length(), structInfo.Name.c_str());
        }
    }
}