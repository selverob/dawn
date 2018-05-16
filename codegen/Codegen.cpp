//
// Created by selvek on 8.5.2018.
//

#include <llvm/Support/Error.h>
#include "Codegen.h"
#include "../ast/expr/Lvalue.h"
#include "../ast/expr/ArrayIdxExpr.h"
#include "../ast/expr/VarExpr.h"
#include "llvm/ADT/APSInt.h"
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
    std::pair<ast::Type *, llvm::Value*> ValInfo;
    if (NamedValues.count(Val->Ident())) {
        ValInfo = NamedValues[Val->Ident()];
    } else {
        if (Globals.count(Val->Ident()))
            ValInfo = Globals[Val->Ident()];
        else
            return nullptr;
    }
    if (auto *ArrIdxExpr = llvm::dyn_cast<ast::ArrayIdxExpr>(Val)) {
        if (!llvm::isa<ast::Array>(ValInfo.first)) {
            LogError(Val->Loc, "Trying to index a scalar variable");
            return nullptr;
        }
        auto ArrStart = llvm::cast<ast::Array>(ValInfo.first)->From->value();
        auto ArrStartConstant = llvm::ConstantInt::get(Module.getContext(), llvm::APSInt::get(ArrStart));
        ArrIdxExpr->Idx->accept(*this);
        if (LastValue == nullptr)
            return nullptr;
        auto *Idx = Builder.CreateSub(LastValue, ArrStartConstant);
        llvm::Value *Idxs[] = {
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(Module.getContext()), 0),
            Idx
        };
        Ptr = Builder.CreateGEP(ValInfo.second, Idxs, "arridx");
    } else {
        Ptr = ValInfo.second;
    }
    return Ptr;
}
