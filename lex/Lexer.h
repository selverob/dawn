//
// Created by selvek on 3.5.2018.
//

#ifndef DAWN_LEXER_H
#define DAWN_LEXER_H

#include <istream>
#include "llvm/Support/MemoryBuffer.h"
#include "llvm/Support/SourceMgr.h"
#include "Lexeme.h"

class Lexer {
public:
    Lexer(llvm::SourceMgr &Sources, unsigned FileIdx);
    Lexer(const Lexer &L) = delete;
    Lexer(Lexer&&);
    Lexeme getLexeme();

private:
    llvm::SourceMgr &Sources;
    char *Current, *End;
    char LastChar;

    void readChar();
    llvm::SMLoc getLoc();

    Lexeme readIdentifier();
    Lexeme readNumber(int base);
    Lexeme readOperator();
};


#endif //DAWN_LEXER_H
