//
// Created by selvek on 3.5.2018.
//

#ifndef DAWN_LEXER_H
#define DAWN_LEXER_H

#include <istream>
#include "llvm/Support/MemoryBuffer.h"
#include "Lexeme.h"

class Lexer {
public:
    Lexer(llvm::MemoryBufferRef Input);
    Lexer(char *Start, char *End);
    Lexeme getLexeme();

private:
    char *Current, *End;
    char LastChar;

    void readChar();

    Lexeme readIdentifier();
    Lexeme readNumber(int base);
    Lexeme readOperator();
};


#endif //DAWN_LEXER_H
