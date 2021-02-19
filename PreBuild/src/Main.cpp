#include <cstdio>
#include <Vision.h>

#include "Lexer.h"

using namespace Vision;

static std::vector<std::string> ParseMetaFiles(const char* metaFilesPath)
{
    FileStream handle = File::Open(metaFilesPath, FileMode::Read);

    std::vector<std::string> metaFiles;

    if (handle.IsOpen())
    {
        std::string line;

        while (File::ReadLine(handle, line))
        {
            metaFiles.push_back(line);
        }

        File::Close(handle);
    }
    else
    {
        printf("unable to open file: %s\n", metaFilesPath);
    }

    return metaFiles;
}

std::vector<std::string> structNames;

static void ParseMember(Lexer* lexer, Token* structName, Token* memberType)
{
    bool isPointer = false;
    bool hasEqual = false;
    bool hasOpenBracket = false;
    bool hasCloseBracket = false;

    Token arrayCountToken;
    Token identifierToken;

    bool parsing = true;

    while (parsing)
    {
        Token token = GetNextToken(lexer);

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
           memberType->TextLength,
           memberType->Text,
           structName->TextLength,
           structName->Text,
           identifierToken.TextLength,
           identifierToken.Text,
           arrayCountToken.TextLength,
           arrayCountToken.Text,
           isPointer,
           (hasOpenBracket && hasCloseBracket));
}

static void ParseStruct(Lexer* lexer)
{
    Token structName = GetNextToken(lexer);

    structNames.push_back(std::string(structName.Text, structName.TextLength));

    while (true)
    {
        Token token = GetNextToken(lexer);

        if (token.Type == TokenType::OpenBrace)
        {
            break;
        }
    }

    printf("StructMember %.*sMembers[] = \n",
            structName.TextLength,
            structName.Text);
    printf("{\n");

    while (true)
    {
        Token token = GetNextToken(lexer);

        if (token.Type == TokenType::CloseBrace)
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

static void ParseInspectableParams(Lexer* lexer)
{
    while (true)
    {
        Token token = GetNextToken(lexer);

        if (token.Type == TokenType::CloseParen || token.Type == TokenType::EndOfStream)
        {
            break;
        }
    }
}

static void ParseInspectable(Lexer* lexer)
{
    Token token = GetNextToken(lexer);

    if (token.Type == TokenType::OpenParen)
    {
        ParseInspectableParams(lexer);

        token = GetNextToken(lexer);

        if (IsTokenEquals(&token, "struct"))
        {
            ParseStruct(lexer);
        }
        else
        {
            printf("unsupported type");
            lexer->At -= token.TextLength;
        }
    }
    else
    {
        printf("missing parentheses. \n");
    }
}

int main()
{
    using namespace Vision;

    const char* metaFilesPath = "../meta_files.txt";
    std::string metaOutputPath = "../Vision/src/Vision/Meta/";

    std::vector<std::string> metaFiles = ParseMetaFiles(metaFilesPath);

    for (uint32 fileIndex = 0; fileIndex < metaFiles.size(); ++fileIndex)
    {
        FileStream handle = File::Open("../" + metaFiles[fileIndex], FileMode::Read);

        std::string contents;

        if (handle.IsOpen())
        {
            printf("parsing file: %s\n", metaFiles[fileIndex].c_str());
            contents = File::ReadContents(handle);
            File::Close(handle);
        }
        else
        {
            printf("unable to open file: %s\n", metaFiles[fileIndex].c_str());
        }

        std::string outputFileName = FileSystem::GetFileName(metaFiles[fileIndex], false);
        std::string outputFilePath = metaOutputPath + outputFileName + "Meta.h";

        freopen(outputFilePath.c_str(), "w", stdout);

        printf("#pragma once\n\n");
        printf("#include \"Vision/Meta/Meta.h\"\n\n");

        Lexer lexer;
        lexer.At = &contents[0];

        bool parsing = true;

        while (parsing)
        {
            Token token = GetNextToken(&lexer);

            switch (token.Type)
            {
                case TokenType::Identifier:
                {
                    if (IsTokenEquals(&token, "Inspect"))
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

    freopen("../Vision/src/Vision/Meta/Meta.h", "w", stdout);

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

    printf("#pragma once\n\n");
    printf("#include \"Vision/Core/Common.h\"\n\n");

    printf("enum MetaType\n{\n");

    for (uint32 typeIndex = 0; typeIndex < VnArrayCount(basicTypes); ++typeIndex)
    {
        printf("\tMetaType_%s,\n", basicTypes[typeIndex]);
    }

    for (auto& structName : structNames)
    {
        printf("\tMetaType_%.*s,\n", structName.length(), structName.c_str());
    }

    printf("};\n\n");

    const char* structMemberString = "struct StructMember\n"
                                     "{\n"
                                     "\tconst char* Name;\n"
                                     "\tMetaType    Type;\n"
                                     "\tuint32      Offset;\n"
                                     "\tuint32      ArrayCount;\n"
                                     "\tbool        IsPointer;\n"
                                     "\tbool        IsArray;\n"
                                     "};\n";
    printf("%s\n", structMemberString);
}