#include "Lexer.h"

namespace Vision
{
    static void EatWhiteSpaceAndComments(Lexer* lexer)
    {
        while (true)
        {
            if (IsWhiteSpace(*lexer->At))
            {
                ++lexer->At;
            }
            else if (lexer->At[0] == '/' && lexer->At[1] == '/')
            {
                lexer->At += 2;

                while (!IsEndOfLine(*lexer->At))
                {
                    ++lexer->At;
                }
            }
            else if (lexer->At[0] == '/' && lexer->At[1] == '*')
            {
                lexer->At += 2;

                while (lexer->At[0] != '*')
                {
                    ++lexer->At;
                }

                lexer->At += 2;
            }
            else
            {
                break;
            }
        }
    }

    Token GetNextToken(Lexer* lexer)
    {
        EatWhiteSpaceAndComments(lexer);

        Token token;
        token.TextLength = 1;
        token.Text = lexer->At;

        switch (*lexer->At)
        {
            case ';':  { token.Type = TokenType::Semicolon;    ++lexer->At; } break;
            case ':':  { token.Type = TokenType::Colon;        ++lexer->At; } break;
            case '*':  { token.Type = TokenType::Asterisk;     ++lexer->At; } break;
            case '(':  { token.Type = TokenType::OpenParen;    ++lexer->At; } break;
            case ')':  { token.Type = TokenType::CloseParen;   ++lexer->At; } break;
            case '[':  { token.Type = TokenType::OpenBracket;  ++lexer->At; } break;
            case ']':  { token.Type = TokenType::CloseBracket; ++lexer->At; } break;
            case '{':  { token.Type = TokenType::OpenBrace;    ++lexer->At; } break;
            case '}':  { token.Type = TokenType::CloseBrace;   ++lexer->At; } break;
            case '=':  { token.Type = TokenType::Equal;        ++lexer->At; } break;
            case '\0': { token.Type = TokenType::EndOfStream;  ++lexer->At; } break;

            case '"':
            {
                ++lexer->At;
                token.Text = lexer->At;

                while (*lexer->At != '"')
                {
                    if (*lexer->At == '\\') // => \"
                    {
                        ++lexer->At;
                    }

                    ++lexer->At;
                }

                token.TextLength = lexer->At - token.Text;
                ++lexer->At;
            }
            break;

            default:
            {
                if (IsAlpha(*lexer->At))
                {
                    while (IsAlpha(*lexer->At) ||
                           IsDigit(*lexer->At) ||
                           *lexer->At == '_' ||
                           *lexer->At == ':')
                    {
                        ++lexer->At;
                    }

                    token.Type = TokenType::Identifier;
                    token.TextLength = lexer->At - token.Text;
                }
                else if (IsDigit(*lexer->At))
                {
                    bool hasFloatingPoint = false;

                    while (IsDigit(*lexer->At))
                    {
                        ++lexer->At;

                        if (*lexer->At == '.' || *lexer->At == 'f')
                        {
                            hasFloatingPoint = true;
                            ++lexer->At;
                        }
                    }

                    token.Type = (hasFloatingPoint) ? TokenType::Float : TokenType::Integer;
                    token.TextLength = lexer->At - token.Text;
                }
                else
                {
                    token.Type = TokenType::Unknown;
                    ++lexer->At;
                }
            }
            break;
        }

        return token;
    }

    bool IsTokenEquals(Token* token, const char* match)
    {
        memorysize matchLength = strlen(match);

        if (token->TextLength != matchLength)
        {
            return false;
        }

        for (uint32 charIndex = 0; charIndex < token->TextLength; charIndex++)
        {
            if (token->Text[charIndex] != match[charIndex])
            {
                return false;
            }
        }

        return true;
    }
}