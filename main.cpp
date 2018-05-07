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
    Sources.AddIncludeFile("code.dawn", llvm::SMLoc(), FullPath);
    llvm::raw_os_ostream Out(std::cout);
    Lexer Lex(Sources, Sources.getMainFileID());
//        Lexeme L = Lex.getLexeme();
//        while (L.K != Lexeme::Kind::ENDOFFILE) {
//            std::cout << L << " ";
//            L = Lex.getLexeme();
//        }
    ast::Parser Parser(Sources, std::move(Lex));
    auto Program = Parser.parse();
    if (!Program)
        return 1;
    ast::Printer Printer(Out);
    Program->accept(Printer);
    Out << '\n';
    Out.flush();
    return 0;
}