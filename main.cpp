#include <iostream>
#include <cstring>
#include <memory>
#include <llvm/IR/Verifier.h>
#include "llvm/Support/TargetRegistry.h"
#include "llvm/Support/TargetSelect.h"
#include "llvm/Target/TargetMachine.h"
#include "llvm/Target/TargetOptions.h"
#include "llvm/Support/Host.h"
#include "llvm/Support/raw_os_ostream.h"
#include "llvm/Support/SourceMgr.h"
#include "llvm/Support/FileSystem.h"
#include "lex/Lexeme.h"
#include "lex/Lexer.h"
#include "ast/Parser.h"
#include "ast/Printer.h"
#include "codegen/Codegen.h"

llvm::StringMap<ast::Prototype*> getStdProtos() {
    llvm::StringMap<ast::Prototype*> Protos;
    Protos["writeln"] = new ast::Prototype(llvm::SMLoc(), std::string("writeln"), "void");
    Protos["writeln"]->addParameter("n", "integer");
    Protos["readln"] = new ast::Prototype(llvm::SMLoc(), std::string("readln"), "integer");
    return Protos;
}

int main(int argc, const char **argv) {
    if (argc != 4) {
        std::cout << "dawn incldir srcfile ofile" << std::endl;
        return 1;
    }
    llvm::InitializeNativeTarget();
    llvm::InitializeNativeTargetAsmPrinter();
    llvm::InitializeNativeTargetAsmParser();
    llvm::SourceMgr Sources;
    std::string FullPath;
    Sources.setIncludeDirs({argv[1]});
    Sources.AddIncludeFile(argv[2], llvm::SMLoc(), FullPath);
    llvm::raw_os_ostream Out(std::cout);
    Lexer Lex(Sources, Sources.getMainFileID());
    /*
    Lexeme L = Lex.getLexeme();
    while (L.K != Lexeme::Kind::ENDOFFILE) {
        std::cout << L << " ";
        L = Lex.getLexeme();
    }
    return 0;*/
    ast::Parser Parser(Sources, std::move(Lex));
    auto Program = Parser.parse();
    if (!Program)
        return 1;
/*   ast::Printer Formatter(Out);
    Program->accept(Formatter);
    Out << '\n';
    Out.flush();*/
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
    codegen::Codegen Generator(Sources, Module, getStdProtos());
    Program->accept(Generator);
    if (!Generator.finishedSuccesfully()) {
        return 2;
    }
    assert(!llvm::verifyModule(Module, &llvm::errs()));
    Module.print(Out, nullptr);
    Out << '\n';
    Out.flush();
    std::error_code EC;
    llvm::raw_fd_ostream Dest(argv[3], EC, llvm::sys::fs::F_None);

    if (EC) {
        llvm::errs() << "Could not open file: " << EC.message();
        return 1;
    }

    llvm::legacy::PassManager PassMgr;

    if (TargetMachine->addPassesToEmitFile(PassMgr, Dest, llvm::TargetMachine::CGFT_ObjectFile)) {
        llvm::errs() << "TargetMachine can't emit a file of this type";
        return 1;
    }

    PassMgr.run(Module);
    Dest.flush();
    return 0;
}
