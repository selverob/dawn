//
// Created by selvek on 8.5.2018.
//

#include <llvm/Support/Error.h>
#include "Codegen.h"
#include "../ast/expr/Lvalue.h"
#include "../ast/expr/ArrayIdxExpr.h"
#include "../ast/expr/VarExpr.h"
#include "llvm/IR/Constants.h"
#include "llvm/IR/InstrTypes.h"
#include "llvm/IR/Function.h"
#include "llvm/IR/Verifier.h"
#include "llvm/Support/Casting.h"
#include "llvm/Transforms/InstCombine/InstCombine.h"
#include "llvm/Transforms/Scalar.h"
#include "llvm/Transforms/Scalar/GVN.h"


codegen::Codegen::Codegen(llvm::SourceMgr &Sources, llvm::Module &Module, llvm::StringMap<ast::Prototype*> StdProtos) :
        Module(Module), Builder(Module.getContext()), Prototypes(std::move(StdProtos)), Sources(Sources), LastValue(nullptr), Finished(true) {
    FPM = std::make_unique<llvm::legacy::FunctionPassManager>(&Module);
    FPM->add(llvm::createPromoteMemoryToRegisterPass());
    FPM->add(llvm::createInstructionCombiningPass());
    FPM->add(llvm::createReassociatePass());
    FPM->add(llvm::createGVNPass());
    FPM->add(llvm::createCFGSimplificationPass());
    FPM->doInitialization();
}

bool codegen::Codegen::finishedSuccesfully() const {
    return Finished;
}

llvm::Value *codegen::Codegen::getLvalueAddress(ast::Lvalue *Val) {
    llvm::Value *Ptr;
    Ptr = NamedValues[Val->Ident()];
    if (!Ptr)
        Ptr = Module.getNamedGlobal(Val->Ident());
    if (!Ptr)
        return nullptr;
    if (auto *ArrIdxExpr = llvm::dyn_cast<ast::ArrayIdxExpr>(Val)) {
        ArrIdxExpr->Idx->accept(*this);
        if (LastValue == nullptr)
            return nullptr;
        llvm::Value *Idxs[] = {
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(Module.getContext()), 0),
            LastValue
        };
        Ptr = Builder.CreateGEP(Ptr, Idxs,"arridx");
    }
    return Ptr;
}
