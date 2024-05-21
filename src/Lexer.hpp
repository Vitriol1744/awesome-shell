/*
 * Created by vitriol1744 on 14.05.2024.
 * Copyright (c) 2024-2024, Szymon Zemke <v1tr10l7@proton.me>
 *
 * SPDX-License-Identifier: GPL-3
 */
#pragma once

#include "Types.hpp"

#include <string>
#include <vector>

enum class TokenType
{
    eNone,
    eSubShell,
    eLeftParen,
    eRightParen,
    eLeftBrace,
    eRightBrace,
    eGreat,
    eLess,
    eGreatGreat,
    eNewLine,
    eGreatAnd,
    ePipe,
    eBackground,
    eWord,
    eAssignmentWord,
    eRedirWord,
    eNumber,
    eString,
    eEnd,

    // Reserved Words
    eIf,
    eThen,
    eElse,
    eElif,
    eFi,
    eCase,
    eEsac,
    eFor,
    eSelect,
    eWhile,
    eUntil,
    eDo,
    eDone,
    eFunction,
    eCoProc,
    eIn,
    eBang,
    eTime,
};

struct Token
{
    Token(TokenType type, std::string_view value, usize line)
        : type(type)
        , value(value)
        , line(line)
    {
    }

    TokenType   type;
    std::string value;
    usize       line = 0;
};

class Lexer
{
  public:
    explicit Lexer(std::string_view code)
        : code(code)
    {
    }

    std::vector<Token> Analyze();

  private:
    std::string        code;
    std::vector<Token> tokens;
    usize              start   = 0;
    usize              current = 0;
    usize              line    = 0;

    void               AddToken(TokenType type, std::string_view value = "");
    void               ParseString();

    bool               IsEndOfFile();
    char               Advance();
    char               Peek();
    bool               Skip(char c);
};
