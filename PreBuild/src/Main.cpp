#include <cstdio>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>

#include "Lexer.h"

struct InspectableParams
{
    std::string Category     = "none";
    bool HandleTypeSwitchCase = true;
};

struct StructInfo
{
    std::string Name;
    InspectableParams Params;
};

std::vector<StructInfo> structs;

static void ParseMember(Lexer* lexer, Token* structName, Token* firstToken)
{
    bool isConst = false;
    bool isStatic = false;
    bool isPointer = false;
    bool hasEqual = false;
    bool hasOpenBracket = false;
    bool hasCloseBracket = false;

    Token memberType;
    Token arrayCountToken;
    Token identifierToken;

    Token token = *firstToken;

    while (true)
    {
        if (token.IsEquals("const"))
        {
            isConst = true;
            memberType = lexer->GetNextToken();
        }
        else if (token.IsEquals("static"))
        {
            isStatic = true;
            memberType = lexer->GetNextToken();
        }
        else
        {
            memberType = token;
            break;
        }

        token = lexer->GetNextToken();
    }

    bool parsing = true;

    while (parsing)
    {
        Token token = lexer->GetNextToken();

        switch (token.Type)
        {
            case TokenType::Asterisk:
            {
                isPointer = true;
            }
            break;

            case TokenType::Identifier:
            {
                if (!hasEqual && !hasOpenBracket)
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

            case TokenType::Equal:
            {
                hasEqual = true;
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

        if (arrayCountToken.TextLength == 0)
        {
            arrayCountToken.TextLength = 1;
            arrayCountToken.Text = (char*)"1";
        }
    }

    printf("\t{ \"%.*s\", MetaType_%.*s, offsetof(Vision::%.*s, %.*s), %.*s, %d, %d },\n",
           identifierToken.TextLength,
           identifierToken.Text,
           memberType.TextLength,
           memberType.Text,
           structName->TextLength,
           structName->Text,
           identifierToken.TextLength,
           identifierToken.Text,
           arrayCountToken.TextLength,
           arrayCountToken.Text,
           isPointer,
           (hasOpenBracket && hasCloseBracket));
}

static void ParseStruct(Lexer* lexer, InspectableParams* params)
{
    Token structName = lexer->GetNextToken();

    StructInfo structInfo;
    structInfo.Name = std::string(structName.Text, structName.TextLength);
    structInfo.Params = *params;

    structs.push_back(structInfo);

    while (true)
    {
        Token token = lexer->GetNextToken();

        if (token.Type == TokenType::OpenBrace)
        {
            break;
        }
    }

    printf("static StructMember %.*sMembers[] = \n",
            structName.TextLength,
            structName.Text);
    printf("{\n");

    while (true)
    {
        Token token = lexer->GetNextToken();

        if (token.Type == TokenType::CloseBrace || token.IsEquals("Ignore"))
        {
            break;
        }
        else
        {
            ParseMember(lexer, &structName, &token);
        }
    }

    printf("};\n\n");
}

static InspectableParams ParseInspectableParams(Lexer* lexer)
{
    InspectableParams params;

    bool hasCategory = false;
    bool hasHandleTypeSwitchCase = false;

    while (true)
    {
        Token token = lexer->GetNextToken();

        if (token.IsEquals("category"))
        {
            hasCategory = true;
        }
        else if (token.IsEquals("handle"))
        {
            hasHandleTypeSwitchCase = true;
        }
        else
        {
            switch (token.Type)
            {
                case TokenType::String:
                {
                    params.Category = std::string(token.Text, token.TextLength);
                }
                break;

                case TokenType::Bool:
                {
                    std::stringstream ss(std::string(token.Text, token.TextLength));
                    ss >> params.HandleTypeSwitchCase;
                }
                break;
            }
        }

        if (token.Type == TokenType::CloseParen || token.Type == TokenType::EndOfStream)
        {
            break;
        }
    }

    return params;
}

static void ParseInspectable(Lexer* lexer)
{
    Token token = lexer->GetNextToken();

    if (token.Type == TokenType::OpenParen)
    {
        InspectableParams params = ParseInspectableParams(lexer);

        token = lexer->GetNextToken();

        if (token.IsEquals("struct"))
        {
            ParseStruct(lexer, &params);
        }
        else
        {
            lexer->At -= token.TextLength;
        }
    }
}

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

int main(int argCount, char* args[])
{
    std::string metaOutputPath = std::string(args[1]);

    for (unsigned int fileIndex = 2; fileIndex < argCount; ++fileIndex)
    {
        std::string path = std::string(args[fileIndex]);
        FILE* handle = fopen((std::string("Vision/src/") + path).c_str(), "r");

        std::string contents;

        if (handle)
        {
            fseek(handle, 0, SEEK_END);
            size_t sizeInBytes = ftell(handle);
            fseek(handle, 0, SEEK_SET);

            contents.resize(sizeInBytes + 1);
            fread(contents.data(), sizeInBytes, 1, handle);
            contents[sizeInBytes] = '\0';

            fclose(handle);
        }

        std::string outputFileName = GetFileName(path, false);
        std::string outputFilePath = metaOutputPath + outputFileName + "Meta.h";

        freopen(outputFilePath.c_str(), "w", stdout);

        printf("#pragma once\n\n");
        printf("#include \"Vision/Meta/Meta.h\"\n\n");
        printf("#include \"%s\"\n\n", path.c_str());

        Lexer lexer;
        lexer.At = &contents[0];

        bool parsing = true;

        while (parsing)
        {
            Token token = lexer.GetNextToken();

            switch (token.Type)
            {
                case TokenType::Identifier:
                {
                    if (token.IsEquals("Inspect"))
                    {
                        ParseInspectable(&lexer);
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
    }

    std::string outputFilePath = metaOutputPath + std::string("Meta.h");
    FILE* handle = fopen(outputFilePath.c_str(), "w");

    const char* basicTypes[] =
    {
        "bool",
        "char",
        "int8",
        "uint8",
        "int16",
        "uint16",
        "int32",
        "uint32",
        "int64",
        "uint64",
        "float32",
        "float64"
    };

    fprintf(handle, "#pragma once\n\n");
    fprintf(handle, "#include \"Vision/Core/Common.h\"\n\n");

    fprintf(handle, "enum MetaType\n{\n");

    for (int typeIndex = 0; typeIndex < sizeof(basicTypes) / sizeof(basicTypes[0]); ++typeIndex)
    {
        fprintf(handle, "\tMetaType_%s,\n", basicTypes[typeIndex]);
    }

    for (auto& structInfo : structs)
    {
        fprintf(handle, "\tMetaType_%.*s,\n", structInfo.Name.length(), structInfo.Name.c_str());
    }

    fprintf(handle, "};\n\n");

    const char* structMemberString = "struct StructMember\n"
                                     "{\n"
                                     "\tconst char* Name;\n"
                                     "\tMetaType    Type;\n"
                                     "\tuint32      Offset;\n"
                                     "\tuint32      ArrayCount;\n"
                                     "\tbool        IsPointer;\n"
                                     "\tbool        IsArray;\n"
                                     "};\n";

    fprintf(handle, "%s\n", structMemberString);

    for (int fileIndex = 2; fileIndex < argCount; ++fileIndex)
    {
        std::string filePath = args[fileIndex];
        std::string fileName = GetFileName(filePath, false);
        fprintf(handle, "#include \"Vision/Meta/%sMeta.h\"\n", fileName.c_str());
    }

    fprintf(handle, "\n#define HandleMetaTypeCases(MemberPointer) \\\n");

    for (auto& structInfo : structs)
    {
        if (structInfo.Params.HandleTypeSwitchCase)
        {
            fprintf(handle, "\tcase MetaType_%.*s: InspectStruct(%.*sMembers, (sizeof(%.*sMembers) / sizeof(%.*sMembers[0])), (%.*s*)MemberPointer, member->Name, memberFn, depth + 1); break; \\\n",
                structInfo.Name.length(), structInfo.Name.c_str(),
                structInfo.Name.length(), structInfo.Name.c_str(),
                structInfo.Name.length(), structInfo.Name.c_str(),
                structInfo.Name.length(), structInfo.Name.c_str(),
                structInfo.Name.length(), structInfo.Name.c_str());
        }
    }

    return 0;
}