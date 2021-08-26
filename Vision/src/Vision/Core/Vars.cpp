#include "pch.hpp"
#include "../Meta/VarsMeta.h"
#include "Vision/Core/Logger.h"
#include "Vision/Core/Vars.h"
#include "Vision/Core/Lexer.h"
#include "Vision/Platform/File.h"

#include <sstream>

namespace Vision
{
    void Vars::Load()
    {
        Vars& vars = *this;

        FileStream handle;
        std::string contents = "";
        std::unordered_map<std::string, std::string> membersTable;

        std::string memberPath = "";

        InspectStruct(VarsMembers,
                      VnArrayCount(VarsMembers),
                      &vars,
                      "vars",
                      [&](const char* structName,
                          StructMember* member,
                          void* memberPointer,
                          bool beginStruct,
                          bool endStruct,
                          int32 arrayIndex,
                          uint32 depth)
        {
            if (depth == 0)
            {
                return;
            }

            if (beginStruct)
            {
                if (depth == 1)
                {
                    std::string fileName = std::string(structName);

                    handle = File::Open(fileName + ".vars", FileMode::Read);

                    if (handle.IsOpen())
                    {
                        contents = File::ReadContents(handle);

                        Lexer lexer;
                        lexer.At = contents.data();

                        bool parsing = true;

                        std::vector<std::string> keyTokens;

                        while (parsing)
                        {
                            Token token = lexer.GetNextToken();

                            switch (token.Type)
                            {
                                case TokenType::EndOfStream:
                                {
                                    parsing = false;
                                }
                                break;

                                case TokenType::Identifier:
                                {
                                    keyTokens.push_back(std::string(token.Text, token.TextLength));
                                }
                                break;

                                case TokenType::OpenBracket:
                                {
                                    keyTokens.push_back("[");
                                }
                                break;

                                case TokenType::CloseBracket:
                                {
                                    keyTokens.push_back("]");
                                }
                                break;

                                case TokenType::UnderScore:
                                {
                                    keyTokens.push_back("_");
                                }
                                break;

                                case TokenType::Integer:
                                case TokenType::Float:
                                case TokenType::String:
                                case TokenType::Bool:
                                {
                                    if (keyTokens.back() == "[")
                                    {
                                        keyTokens.push_back(std::string(token.Text, token.TextLength));
                                    }
                                    else
                                    {
                                        std::string value = std::string(token.Text, token.TextLength);
                                        std::string key;

                                        for (auto& token : keyTokens)
                                        {
                                            key = key + token;
                                        }

                                        membersTable[key] = value;

                                        keyTokens.clear();
                                    }
                                }
                                break;
                            }
                        }

                        File::Close(handle);
                    }
                    else
                    {
                        VnCoreInfo("unable to open file: {0}", fileName);
                    }
                }

                if (depth > 1)
                {
                    std::string arraySubscript = "";

                    if (arrayIndex != -1)
                    {
                        std::stringstream ss;
                        ss << arrayIndex;
                        arraySubscript = "[" + ss.str()  + "]";
                    }

                    memberPath += std::string(structName) + arraySubscript + std::string("_");
                }
            }
            else if (endStruct)
            {
                if (depth == 1)
                {
                    if (handle.IsOpen())
                    {
                        membersTable.clear();
                    }
                }

                if (depth > 1)
                {
                    int arrayIndexDigitCount = 0;

                    if (arrayIndex != -1)
                    {
                        std::stringstream ss;
                        ss << arrayIndex;
                        arrayIndexDigitCount = static_cast<uint32>(ss.str().length() + 2);
                    }

                    memberPath.resize(memberPath.length() - (strlen(structName) + 1 + arrayIndexDigitCount));
                }
            }
            else
            {
                std::string arraySubscript = "";

                if (arrayIndex != -1)
                {
                    std::stringstream ss;
                    ss << arrayIndex;

                    arraySubscript = "[" + ss.str()  + "]";
                }

                auto it = membersTable.find(memberPath + member->Name + arraySubscript);

                if (it != membersTable.end())
                {
                    const std::string& value = it->second;
                    std::stringstream stringStream(value);

                    switch (member->Type)
                    {
                        case MetaType_String:
                        {
                            String* string = (String*)memberPointer;
                            AssignString(string, value.data(), value.length());
                        }
                        break;

                        case MetaType_char:
                        {
                            if (member->TypeInfoFlags & TypeInfo_IsPointer)
                            {
                                char** text = (char**)(void**)memberPointer;

                                if (*text)
                                {
                                    delete[] *text;
                                    *text = nullptr;
                                }

                                *text = new char[value.length() + 1];
                                strcpy(*text, value.c_str());
                            }
                            else if (member->TypeInfoFlags & TypeInfo_IsArray)
                            {
                                char* memberPtr = (char*)memberPointer;
                                memcpy(memberPtr, value.data(), value.length() + 1);
                            }
                        }
                        break;

                        case MetaType_bool:
                        {
                            bool* memberPtr = (bool*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_int16:
                        {
                            int16* memberPtr = (int16*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_uint16:
                        {
                            uint16* memberPtr = (uint16*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_int32:
                        {
                            int32* memberPtr = (int32*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_uint32:
                        {
                            uint32* memberPtr = (uint32*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_int64:
                        {
                            int64* memberPtr = (int64*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_uint64:
                        {
                            uint64* memberPtr = (uint64*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_float32:
                        {
                            float32* memberPtr = (float32*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;

                        case MetaType_float64:
                        {
                            float64* memberPtr = (float64*)memberPointer;
                            stringStream >> *memberPtr;
                        }
                        break;
                    }
                }
                else
                {
                    VnCoreInfo("missing value for member: {0} in {1}.vars using default value",
                                memberPath + member->Name + arraySubscript,
                                structName);
                }
            }
        });
    }

    void Vars::Save()
    {
        Vars& vars = *this;

        FileStream handle;
        std::stringstream stringStream;

        std::string memberPath = "";

        InspectStruct(VarsMembers,
                      VnArrayCount(VarsMembers),
                      &vars,
                      "vars",
                      [&](const char* structName,
                          StructMember* member,
                          void* memberPointer,
                          bool beginStruct,
                          bool endStruct,
                          int32 arrayIndex,
                          uint32 depth)
        {
            if (depth == 0)
            {
                return;
            }

            if (beginStruct)
            {
                if (depth == 1)
                {
                    handle = File::Open(std::string(structName) + ".vars", FileMode::Write);

                    if (!handle.IsOpen())
                    {
                        VnCoreInfo("unable to open file: {0}.vars", structName);
                    }
                }

                if (depth > 1)
                {
                    std::string arraySubscript = "";

                    if (arrayIndex != -1)
                    {
                        std::stringstream ss;
                        ss << arrayIndex;
                        arraySubscript = "[" + ss.str()  + "]";
                    }

                    memberPath += std::string(structName) + arraySubscript + std::string("_");
                }
            }
            else if (endStruct)
            {
                if (depth == 1)
                {
                    if (handle.IsOpen())
                    {
                        File::WriteContents(handle, stringStream.str());
                        stringStream.clear();
                        File::Close(handle);
                    }
                }

                if (depth > 1)
                {
                    uint32 arrayIndexDigitCount = 0;

                    if (arrayIndex != -1)
                    {
                        std::stringstream ss;
                        ss << arrayIndex;
                        arrayIndexDigitCount = static_cast<uint32>(ss.str().length() + 2);
                    }

                    memberPath.resize(memberPath.length() - (strlen(structName) + 1 + arrayIndexDigitCount));
                }
            }
            else
            {
                std::string arraySubscript = "";

                if (arrayIndex != -1)
                {
                    std::stringstream ss;
                    ss << arrayIndex;

                    arraySubscript = "[" + ss.str()  + "]";
                }

                switch (member->Type)
                {
                    case MetaType_char:
                    {
                        char* memberPtr = (char*)(*(void**)(memberPointer));

                        if (member->TypeInfoFlags & TypeInfo_IsPointer || member->TypeInfoFlags & TypeInfo_IsArray)
                        {
                            stringStream << memberPath << member->Name << arraySubscript << " \"" << memberPtr << "\"\n";
                        }
                        else
                        {
                            stringStream << memberPath << member->Name << arraySubscript << " \"" << *memberPtr << "\"\n";
                        }
                    }
                    break;

                    case MetaType_String:
                    {
                        String* memberPtr = (String*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " \"" << memberPtr->Data << "\"\n";
                    }
                    break;

                    case MetaType_bool:
                    {
                        bool* memberPtr = (bool*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_int16:
                    {
                        int16* memberPtr = (int16*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_uint16:
                    {
                        uint16* memberPtr = (uint16*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_int32:
                    {
                        int32* memberPtr = (int32*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_uint32:
                    {
                        uint32* memberPtr = (uint32*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_int64:
                    {
                        int64* memberPtr = (int64*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_uint64:
                    {
                        uint64* memberPtr = (uint64*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_float32:
                    {
                        float32* memberPtr = (float32*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;

                    case MetaType_float64:
                    {
                        float64* memberPtr = (float64*)memberPointer;
                        stringStream << memberPath << member->Name << arraySubscript << " " << *memberPtr << "\n";
                    }
                    break;
                }
            }
        });
    }
}