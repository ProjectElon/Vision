#pragma once

#include "Vision/Core/Common.h"
#include "Vision/Meta/Meta.h"

namespace Vision
{
    enum class TokenType
    {
        Unknown,

        Semicolon,
        Colon,
        Asterisk,
        OpenParen,
        CloseParen,
        OpenBracket,
        CloseBracket,
        OpenBrace,
        CloseBrace,
        Equal,

        String,
        Identifier,
        Integer,
        Float,
        Bool,

        EndOfStream,
        Count
    };

    struct Token
    {
        TokenType Type = TokenType::Unknown;

        memorysize TextLength = 0;
        char* Text = nullptr;

        bool IsEquals(const char* match);
    };

    struct Lexer
    {
        char* At = nullptr;

        Token GetNextToken();
    };

    inline bool IsEndOfLine(char c)
    {
        return c == '\n' || c == '\r';
    }

    inline bool IsWhiteSpace(char c)
    {
        return c == ' ' || c == '\t' || IsEndOfLine(c);
    }

    inline bool IsAlpha(char c)
    {
        return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
    }

    inline bool IsDigit(char c)
    {
        return c >= '0' && c <= '9';
    }

    using MemberInspectionFn = std::function<void(const char* structName,
                                                  StructMember* member,
                                                  void* memberPointer,
                                                  bool beginStruct,
                                                  bool endStruct,
                                                  uint32 depth)>;

    template<typename T>
    void InspectStruct(StructMember* members,
                       uint32 memebersCount,
                       T* structPointer,
                       const char* structName,
                       MemberInspectionFn memberFn,
                       uint32 depth = 0)
    {
        memberFn(structName,
                 nullptr,
                 structPointer,
                 true,
                 false,
                 depth);

        for (uint32 memberIndex = 0; memberIndex < memebersCount; ++memberIndex)
        {
            StructMember* member = members + memberIndex;
            void* memberPointer = (uint8*)structPointer + member->Offset;

            switch (member->Type)
            {
                HandleMetaTypeCases(memberPointer)

                default:
                {
                    memberFn(structName,
                             member,
                             memberPointer,
                             false,
                             false,
                             depth);
                }
                break;
            }
        }

        memberFn(structName,
                 nullptr,
                 structPointer,
                 false,
                 true,
                 depth);
    }
}