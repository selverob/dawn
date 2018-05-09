#include <iostream>
#include <cstring>
#include <memory>
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "lex/Lexeme.h"
#include "lex/Lexer.h"
#include "ast/Parser.h"
#include "ast/Printer.h"
#include "codegen/Codegen.h"

int main() {
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
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
    codegen::Codegen Generator(Sources, llvm::sys::getDefaultTargetTriple());
    Generator.visit(*Program->Functions[0]);
    Generator.print(Out);
    Out << '\n';
    Out.flush();
    return 0;
}