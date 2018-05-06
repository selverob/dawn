#include <iostream>
#include <cstring>
#include <memory>
#include "llvm/Support/raw_os_ostream.h"
#include "lex/Lexeme.h"
#include "lex/Lexer.h"
#include "ast/Parser.h"
#include "ast/Printer.h"

int main() {
    std::unique_ptr<char[]> Buf(new char[280]);
    llvm::raw_os_ostream Out(std::cout);
    while (!std::cin.eof()) {
        std::cin.getline(Buf.get(), 280);
        size_t Len = std::strlen(Buf.get());
        Lexer Lex(Buf.get(), Buf.get() + Len);
        /*Lexeme L = Lex.getLexeme();
        while (L.K != Lexeme::Kind::ENDOFFILE) {
            std::cout << L.K << " ";
            L = Lex.getLexeme();
        }*/
        ast::Parser Parser(std::move(Lex));
        auto expr = Parser.parseExpr();
        ast::Printer Printer(Out);
        expr->accept(Printer);
        Out << '\n';
        Out.flush();
    }
    return 0;
}