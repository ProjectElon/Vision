#pragma once

#include "Vision/Core/Defines.h"
#include "../Meta/Meta.h"

namespace Vision
{
    enum class TokenType
    {
        Unknown,
        UnderScore,
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
                                                  int32 arrayIndex,
                                                  uint32 depth)>;

    void InspectStruct(StructMember* members,
                       uint32 memebersCount,
                       void* structPointer,
                       const char* structName,
                       MemberInspectionFn memberFn,
                       int32 arrayIndex = -1,
                       uint32 depth = 0);

    #define InspectMembers(Struct, Instance, MemberInspectFn) InspectStruct(Struct##Members, VnArrayCount(Struct##Members), &Instance, #Instance, MemberInspectFn)
}