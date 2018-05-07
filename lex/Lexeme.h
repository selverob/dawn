//
// Created by selvek on 3.5.2018.
//

#ifndef DAWN_LEXEME_H
#define DAWN_LEXEME_H

#include <iostream>
#include <string>

#include "llvm/Support/SMLoc.h"

struct Lexeme {
    enum struct Kind {
        UNKNOWN,
        ENDOFFILE,
        PROGRAM,
        CONST,
        VAR,
        FUNCTION,
        IF,
        THEN,
        ELSE,
        WHILE,
        DO,
        FOR,
        TO,
        DOWNTO,
        BEGIN,
        END,
        EXIT,
        AND,
        OR,
        NOT,
        LT,
        LTE,
        EQ,
        GTE,
        GT,
        NEQ,
        PLUS,
        MINUS,
        MOD,
        MUL,
        DIV,
        LPAR,
        RPAR,
        ASSIGN,
        NUMBER,
        IDENT,
    };

    Kind K;
    llvm::SMLoc Loc;
    char Char;
    std::string IdentifierStr;
    long NumericValue;

    explicit Lexeme(Kind K, llvm::SMLoc Position);
    Lexeme(Kind K, char Char, llvm::SMLoc Position);
    Lexeme(Kind K, const std::string &IdentifierStr, llvm::SMLoc Position);
    Lexeme(Kind K, long NumericValue, llvm::SMLoc Position);
};

std::ostream &operator<< (std::ostream& Stream, const Lexeme::Kind &K);
std::ostream &operator<< (std::ostream& stream, const Lexeme &l);

#endif //DAWN_LEXEME_H
