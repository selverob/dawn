#include <iostream>
#include <cstring>
#include <memory>
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
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

    auto TargetTriple = llvm::sys::getDefaultTargetTriple();
    std::string Error;
    auto Target = llvm::TargetRegistry::lookupTarget(TargetTriple, Error);
    if (!Target)
        throw Error;

    llvm::TargetOptions Opt;
    auto RM = llvm::Optional<llvm::Reloc::Model>();
    auto TargetMachine = Target->createTargetMachine(TargetTriple, "generic", "", Opt, RM);
    llvm::LLVMContext Context;
    llvm::Module Module("top level module", Context);
    Module.setDataLayout(TargetMachine->createDataLayout());
    Module.setTargetTriple(TargetTriple);
    codegen::Codegen Generator(Sources, Module);
    Generator.visit(*Program->Functions[0]);
    Module.print(Out, nullptr);
    Out << '\n';
    Out.flush();
    return 0;
}