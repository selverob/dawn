#include <iostream>
#include <cstring>
#include <memory>
#include "lex/Lexeme.h"
#include "lex/Lexer.h"
#include "ast/Parser.h"

int main() {
    std::unique_ptr<char[]> Buf(new char[280]);
    while (!std::cin.eof()) {
        std::cin.getline(Buf.get(), 280);
        size_t Len = std::strlen(Buf.get());
        Lexer Lex(Buf.get(), Buf.get() + Len);
        ast::Parser Parser(std::move(Lex));
        auto expr = Parser.parseExpr();
        std::cout << std::endl;
    }
    return 0;
}