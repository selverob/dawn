//
// Created by selvek on 8.5.2018.
//

#include <llvm/Support/Error.h>
#include "Codegen.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"


codegen::Codegen::Codegen(llvm::SourceMgr &Sources, llvm::Module &Module, llvm::StringMap<ast::Prototype*> StdProtos) :
        Module(Module), LastValue(nullptr), Builder(Module.getContext()), Prototypes(std::move(StdProtos)), Sources(Sources), Finished(true) {
    FPM = std::make_unique<llvm::legacy::FunctionPassManager>(&Module);
//    FPM->add(llvm::createPromoteMemoryToRegisterPass());
//    FPM->add(llvm::createInstructionCombiningPass());
//    FPM->add(llvm::createReassociatePass());
//    FPM->add(llvm::createGVNPass());
//    FPM->add(llvm::createCFGSimplificationPass());
    FPM->doInitialization();
}

bool codegen::Codegen::finishedSuccesfully() const {
    return Finished;
}
