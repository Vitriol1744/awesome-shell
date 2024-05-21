/*
 * Created by vitriol1744 on 14.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#include "Lexer.hpp"

#include "Logger.hpp"

std::vector<Token> Lexer::Analyze()
{
    using TT = TokenType;

    while (!IsEndOfFile())
    {
        start  = current;
        char c = Advance();
        switch (c)
        {
            case '#':
                while (Peek() != '\n' && !IsEndOfFile()) Advance();
                break;
            case '(':
                if (!isalpha(Peek()))
                {
                    LogError("Unrecognized character inside of subshell!");
                    continue;
                }
                while (Peek() != ')')
                {
                    if (Peek() == '\n' || IsEndOfFile())
                        LogError("Unterminated subshell");
                    Advance();
                }
                AddToken(TT::eSubShell,
                         code.substr(start + 1, current - start - 1));
                Advance();
                break;
            case ')': AddToken(TT::eRightParen); break;
            case '{': AddToken(TT::eLeftBrace); break;
            case '}': AddToken(TT::eRightBrace); break;
            case ';': AddToken(TT::eEnd); break;
            case ' ':
            case '\r':
            case '\t': break;
            case '\n': line++; break;
            case '\'':
            case '"': ParseString(); break;

            default: break;
        }
    }

    return tokens;
}

void Lexer::AddToken(TokenType type, std::string_view literal)
{
    tokens.emplace_back(
        type, literal.empty() ? code.substr(start, current) : literal, line);
}
void Lexer::ParseString()
{
    while (Peek() != '"' && !IsEndOfFile())
    {
        if (Peek() == '\n') line++;
        Advance();
    }
    if (IsEndOfFile())
    {
        LogError("Unterminated string");
        return;
    }
    Advance();
    AddToken(TokenType::eString, code.substr(start + 1, current - 1));
}
bool Lexer::IsEndOfFile() { return current >= code.size(); }
char Lexer::Advance() { return code[current++]; }
char Lexer::Peek() { return code[current]; }
bool Lexer::Skip(char c)
{
    if (code[current] == c)
    {
        current++;
        return true;
    }
    return false;
}
