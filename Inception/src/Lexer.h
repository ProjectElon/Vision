#pragma once

#include <cstdint>
#include <cstdio>

#include "Common.h"

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
    UnderScore,

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

    String Text;

    bool IsEquals(const char* match);
};

struct Lexer
{
    String Input;
    char At[2] = { 0 };

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