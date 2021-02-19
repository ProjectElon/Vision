#pragma once

#include "Vision/Core/Common.h"

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

        EndOfStream,
        Count
    };

    struct Token
    {
        TokenType Type = TokenType::Unknown;

        memorysize TextLength = 0;
        char* Text = nullptr;
    };

    struct Lexer
    {
        char* At = nullptr;
    };

    Token GetNextToken(Lexer* lexer);

    bool IsTokenEquals(Token* token, const char* match);

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
}