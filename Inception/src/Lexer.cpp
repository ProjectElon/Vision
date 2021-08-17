#include "Lexer.h"

#include <cstring>
#include <cassert>

static void AdvanceLexer(Lexer* lexer, uint32_t count)
{
    lexer->Input.Data += count;
    lexer->Input.Count -= count;

    if (lexer->Input.Count <= 0)
    {
        lexer->At[0] = 0;
        lexer->At[1] = 0;
    }
    else if (lexer->Input.Count == 1)
    {
        lexer->At[0] = lexer->Input.Data[0];
        lexer->At[1] = 0;
    }
    else
    {
        lexer->At[0] = lexer->Input.Data[0];
        lexer->At[1] = lexer->Input.Data[1];
    }
}

static void EatWhiteSpaceAndComments(Lexer* lexer)
{
    while (true)
    {
        if (IsWhiteSpace(lexer->At[0]))
        {
            AdvanceLexer(lexer, 1);
        }
        else if (lexer->At[0] == '/' && lexer->At[1] == '/')
        {
            AdvanceLexer(lexer, 2);

            while (!IsEndOfLine(lexer->At[0]))
            {
                AdvanceLexer(lexer, 1);
            }
        }
        else if (lexer->At[0] == '/' && lexer->At[1] == '*')
        {
            AdvanceLexer(lexer, 2);

            while (lexer->At[0] != '*')
            {
                AdvanceLexer(lexer, 1);
            }

            AdvanceLexer(lexer, 2);
        }
        else
        {
            break;
        }
    }
}

Token Lexer::GetNextToken()
{
    EatWhiteSpaceAndComments(this);

    Token token;
    token.Text = { Input.Data, 1 };

    switch (At[0])
    {
        case ';':
        {
            token.Type = TokenType::Semicolon;
            AdvanceLexer(this, 1);
        }
        break;

        case ':':
        {
            token.Type = TokenType::Colon;
            AdvanceLexer(this, 1);
        }
        break;

        case '*':
        {
            token.Type = TokenType::Asterisk;
            AdvanceLexer(this, 1);
        }
        break;

        case '(':
        {
            token.Type = TokenType::OpenParen;
            AdvanceLexer(this, 1);
        }
        break;

        case ')':
        {
            token.Type = TokenType::CloseParen;
            AdvanceLexer(this, 1);
        }
        break;

        case '[':
        {
            token.Type = TokenType::OpenBracket;
            AdvanceLexer(this, 1);
        }
        break;

        case ']':
        {
            token.Type = TokenType::CloseBracket;
            AdvanceLexer(this, 1);
        }
        break;

        case '{':
        {
            token.Type = TokenType::OpenBrace;
            AdvanceLexer(this, 1);
        }
        break;

        case '}':
        {
            token.Type = TokenType::CloseBrace;
            AdvanceLexer(this, 1);
        }
        break;

        case '=':
        {
            token.Type = TokenType::Equal;
            AdvanceLexer(this, 1);
        }
        break;

        case '\0':
        {
            token.Type = TokenType::EndOfStream;
            AdvanceLexer(this, 1);
        }
        break;

        case '_':
        {
            token.Type = TokenType::UnderScore;
            AdvanceLexer(this, 1);
        }
        break;

        case '"':
        {
            AdvanceLexer(this, 1);
            token.Text.Data = Input.Data;

            while (At[0] != '"')
            {
                if (At[0] == '\\') // => \"
                {
                    AdvanceLexer(this, 1);
                }

                AdvanceLexer(this, 1);
            }

            token.Type = TokenType::String;
            token.Text.Count = Input.Data - token.Text.Data;
            AdvanceLexer(this, 1);
        }
        break;

        default:
        {
            if (IsAlpha(At[0]))
            {
                while (IsAlpha(At[0]) ||
                        IsDigit(At[0]) ||
                        At[0] == '_' ||
                        At[0] == ':') // : for namespaces
                {
                    AdvanceLexer(this, 1);
                }

                token.Text.Count = Input.Data - token.Text.Data;

                if (token.IsEquals("true") || token.IsEquals("false"))
                {
                    token.Type = TokenType::Bool;
                }
                else
                {
                    token.Type = TokenType::Identifier;
                }
            }
            else if (IsDigit(At[0]))
            {
                bool hasFloatingPoint = false;

                while (IsDigit(At[0]))
                {
                    AdvanceLexer(this, 1);

                    if (At[0] == '.' || At[0] == 'f' || At[0] == 'lf')
                    {
                        hasFloatingPoint = true;
                        AdvanceLexer(this, 1);
                    }
                }

                token.Type = (hasFloatingPoint) ? TokenType::Float : TokenType::Integer;
                token.Text.Count = Input.Data - token.Text.Data;
            }
            else
            {
                token.Type = TokenType::Unknown;
                AdvanceLexer(this, 1);
            }
        }
        break;
    }

    return token;
}

bool Token::IsEquals(const char* match)
{
    size_t matchCount = strlen(match);

    if (Text.Count != matchCount)
    {
        return false;
    }

    for (uint32_t charIndex = 0; charIndex < Text.Count; charIndex++)
    {
        if (Text.Data[charIndex] != match[charIndex])
        {
            return false;
        }
    }

    return true;
}