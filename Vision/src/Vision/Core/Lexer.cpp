#include "pch.hpp"
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

    Token Lexer::GetNextToken()
    {
        EatWhiteSpaceAndComments(this);

        Token token;
        token.TextLength = 1;
        token.Text = At;

        switch (*At)
        {
            case ';':  { token.Type = TokenType::Semicolon;    ++At; } break;
            case ':':  { token.Type = TokenType::Colon;        ++At; } break;
            case '*':  { token.Type = TokenType::Asterisk;     ++At; } break;
            case '(':  { token.Type = TokenType::OpenParen;    ++At; } break;
            case ')':  { token.Type = TokenType::CloseParen;   ++At; } break;
            case '[':  { token.Type = TokenType::OpenBracket;  ++At; } break;
            case ']':  { token.Type = TokenType::CloseBracket; ++At; } break;
            case '{':  { token.Type = TokenType::OpenBrace;    ++At; } break;
            case '}':  { token.Type = TokenType::CloseBrace;   ++At; } break;
            case '=':  { token.Type = TokenType::Equal;        ++At; } break;
            case '\0': { token.Type = TokenType::EndOfStream;  ++At; } break;
            case '_':  { token.Type = TokenType::UnderScore; ++At; } break;

            case '"':
            {
                ++At;
                token.Text = At;

                while (*At != '"')
                {
                    if (*At == '\\') // => \"
                    {
                        ++At;
                    }

                    ++At;
                }
                token.Type = TokenType::String;
                token.TextLength = At - token.Text;
                ++At;
            }
            break;

            default:
            {
                if (IsAlpha(*At))
                {
                    while (IsAlpha(*At) ||
                           IsDigit(*At) ||
                           *At == '_' ||
                           *At == ':')
                    {
                        ++At;
                    }

                    token.TextLength = At - token.Text;

                    if (token.IsEquals("true") || token.IsEquals("false"))
                    {
                        token.Type = TokenType::Bool;
                    }
                    else
                    {
                        token.Type = TokenType::Identifier;
                    }
                }
                else if (IsDigit(*At))
                {
                    bool hasFloatingPoint = false;

                    while (IsDigit(*At))
                    {
                        ++At;

                        if (*At == '.' || *At == 'f')
                        {
                            hasFloatingPoint = true;
                            ++At;
                        }
                    }

                    token.Type = (hasFloatingPoint) ? TokenType::Float : TokenType::Integer;
                    token.TextLength = At - token.Text;
                }
                else
                {
                    token.Type = TokenType::Unknown;
                    ++At;
                }
            }
            break;
        }

        return token;
    }

    bool Token::IsEquals(const char* match)
    {
        memorysize matchLength = strlen(match);

        if (TextLength != matchLength)
        {
            return false;
        }

        for (uint32 charIndex = 0; charIndex < TextLength; charIndex++)
        {
            if (Text[charIndex] != match[charIndex])
            {
                return false;
            }
        }

        return true;
    }


    void InspectStruct(StructMember* members,
                           uint32 memebersCount,
                           void* structPointer,
                           const char* structName,
                           MemberInspectionFn memberFn,
                           int32 arrayIndex,
                           uint32 depth)
    {
        memberFn(structName,
                 nullptr,
                 structPointer,
                 true,
                 false,
                 arrayIndex,
                 depth);

        for (uint32 memberIndex = 0;
             memberIndex < memebersCount;
             ++memberIndex)
        {
            StructMember* member = members + memberIndex;

            if ((member->TypeInfoFlags & TypeInfo_IsArray) && member->Type != MetaType_char)
            {
                for (uint32 arrayIndex = 0;
                     arrayIndex < member->ArrayCount;
                     ++arrayIndex)
                {
                    void* memberPointer = (void*)(((uint8*)structPointer + member->Offset) + arrayIndex * member->TypeSize);

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
                                     arrayIndex,
                                     depth);
                        } break;
                    }
                }
            }
            else
            {
                int32 arrayIndex = -1;

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
                                 -1,
                                 depth);
                    } break;
                }
            }
        }

        memberFn(structName,
                 nullptr,
                 structPointer,
                 false,
                 true,
                 arrayIndex,
                 depth);
    }
}