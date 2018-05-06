#include <iostream>
#include <cstring>
#include <memory>
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "lex/Lexeme.h"
#include "lex/Lexer.h"
#include "ast/Parser.h"
#include "ast/Printer.h"

int main() {
    llvm::SourceMgr Sources;
    std::string FullPath;
    Sources.setIncludeDirs({"/home/selvek/FIT/PJP/dawn"});
    auto Idx = Sources.AddIncludeFile("code.dawn", llvm::SMLoc(), FullPath);
    llvm::raw_os_ostream Out(std::cout);
    Lexer Lex(Sources.getMemoryBuffer(Idx)->getMemBufferRef());
//        Lexeme L = Lex.getLexeme();
//        while (L.K != Lexeme::Kind::ENDOFFILE) {
//            std::cout << L << " ";
//            L = Lex.getLexeme();
//        }
    ast::Parser Parser(std::move(Lex));
    auto expr = Parser.parseStmt();
    ast::Printer Printer(Out);
    expr->accept(Printer);
    Out << '\n';
    Out.flush();
    return 0;
}