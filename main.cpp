#include <iostream>
#include <cstring>
#include <memory>
#include "lex/Lexeme.h"
#include "lex/Lexer.h"

int main() {
    std::unique_ptr<char[]> Buf(new char[280]);
    while (!std::cin.eof()) {
        std::cin.getline(Buf.get(), 280);
        size_t Len = std::strlen(Buf.get());
        Lexer Lex(Buf.get(), Buf.get() + Len);
        Lexeme Token = Lex.getLexeme();
        while (Token.K != Lexeme::Kind::ENDOFFILE) {
            std::cout << Token << ' ';
            Token = Lex.getLexeme();
        }
        std::cout << std::endl;
    }
    return 0;
}