//
// Created by selvek on 15.5.2018.
//

#include "llvm/IR/Verifier.h"
#include "Codegen.h"
#include "../ast/decl/Consts.h"
#include "../ast/decl/Program.h"
#include "../ast/type/Void.h"
#include "../ast/type/Integer.h"
#include "../ast/type/Array.h"
#include "../ast/type/NumericArrayBound.h"
#include "../ast/type/IdentifierArrayBound.h"


void codegen::Codegen::visit(ast::Consts &E) {
    for (const auto &Const : E.Constants) {
        Constants[Const.first] = llvm::ConstantInt::get(Module.getContext(),
                                                        llvm::APInt(64, (uint64_t) Const.second, true));
    }
}

void codegen::Codegen::visit(ast::Prototype &E) {
    Prototypes[E.Name] = &E;
}

void codegen::Codegen::visit(ast::Function &E) {
    //TODO: Handle redefinition errors
    lookupFunction(E.Proto->Name);
    if (!LastValue)
        generatePrototype(*E.Proto);
    auto F = llvm::dyn_cast<llvm::Function>(LastValue);

    llvm::BasicBlock *Body = llvm::BasicBlock::Create(Module.getContext(), "entry", F);
    Builder.SetInsertPoint(Body);

    llvm::StringMap<std::pair<ast::Type*, llvm::AllocaInst *>> OldNamedValues;
    std::swap(NamedValues, OldNamedValues);
    size_t i = 0;
    for (auto &Arg : F->args()) {
        auto &Param = E.Proto->Parameters[i];
        auto Err = createAlloca(F, Param.first, Param.second);
        assert(Err.success());
        Builder.CreateStore(&Arg, NamedValues[Param.first].second);
        i++;
    }

    for (const auto &VarInfo : E.Variables->Variables) {
        auto Err = createAlloca(F, VarInfo.first, VarInfo.second);
        if (Err) {
            return LogError(E.Variables->Loc, llvm::Twine("Invalid declaration of variable", VarInfo.first).getSingleStringRef());
        }
    }

    if (!llvm::isa<ast::Void>(E.Proto->ReturnType)) {
        llvm::Error Err = createAlloca(F, F->getName(), E.Proto->ReturnType);
        if (Err)
            return LogError(E.Loc, "Couldn't create return type allocation");
    }

    llvm::BasicBlock *RetBlock = llvm::BasicBlock::Create(Module.getContext(), "return", F);
    Builder.SetInsertPoint(RetBlock);
    if (!llvm::isa<ast::Void>(E.Proto->ReturnType)) {
        auto ReturnResult = Builder.CreateLoad(NamedValues[F->getName()].second);
        Builder.CreateRet(ReturnResult);
    } else {
        Builder.CreateRetVoid();
    }

    Builder.SetInsertPoint(Body);
    E.Body->accept(*this);
    if (LastValue == nullptr)
        return;
    Builder.CreateBr(RetBlock);

    assert(!llvm::verifyFunction(*F, &llvm::errs()));
    FPM->run(*F);
    std::swap(NamedValues, OldNamedValues);

    LastValue = F;
}

void codegen::Codegen::visit(ast::Program &E) {
    E.Constants->accept(*this);
    E.Variables->accept(*this);
    for (auto &P : E.Prototypes) {
        P->accept(*this);
    }
    for (auto &F : E.Functions) {
        F->accept(*this);
        if (LastValue == nullptr)
            return;
    }

    auto Loc = E.Body->Loc;

    auto Proto = std::make_unique<ast::Prototype>(Loc, "main", ast::Void::get());

    ast::Function Fn(
            Loc,
            std::move(Proto),
            std::make_unique<ast::Vars>(Loc),
            std::move(E.Body));
    Fn.accept(*this);
}

void codegen::Codegen::visit(ast::Vars &E) {
    for (auto &Var : E.Variables) {
        auto LLVMType = getLLVMType(Var.second);
        if (!LLVMType)
            return LogError(E.Loc, "Unable to generate type for a variable");
        Module.getOrInsertGlobal(Var.first, *LLVMType);
        auto GlobalVar = Module.getNamedGlobal(Var.first);
        GlobalVar->setLinkage(llvm::GlobalValue::InternalLinkage);
        if (llvm::isa<ast::Integer>(Var.second)) {
            GlobalVar->setInitializer(llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 0)));
        } else if (llvm::isa<ast::Array>(Var.second)) {
            GlobalVar->setInitializer(llvm::ConstantAggregateZero::get(*LLVMType));
        } else {
            return LogError(E.Loc, "Invalid variable type");
        }
        Globals[Var.first] = std::pair(Var.second, GlobalVar);
    }
}

void codegen::Codegen::generatePrototype(ast::Prototype &P) {
    for (const auto &Param : P.Parameters) {
        if (!llvm::isa<ast::Integer>(Param.second))
            return LogError(P.Loc, "Only integer parameters are supported");
    }
    std::vector<llvm::Type *> Ints(P.Parameters.size(), llvm::Type::getInt64Ty(Module.getContext()));
    llvm::Type *ReturnType;
    if (llvm::isa<ast::Integer>(P.ReturnType))
        ReturnType = llvm::Type::getInt64Ty(Module.getContext());
    else if (llvm::isa<ast::Void>(P.ReturnType))
        ReturnType = llvm::Type::getVoidTy(Module.getContext());
    else
        return LogError(P.Loc, "Returning arrays from functions is not supported");
    llvm::FunctionType *FT = llvm::FunctionType::get(ReturnType, Ints, false);

    auto *F = llvm::Function::Create(FT, llvm::Function::ExternalLinkage, P.Name, &Module);
    size_t i = 0;
    for (auto &Param : F->args())
        Param.setName(P.Parameters[i++].first);
    LastValue = F;
}

llvm::Error codegen::Codegen::createAlloca(llvm::Function *F, llvm::StringRef VarName, ast::Type *VarType) {
    llvm::IRBuilder<> B(&F->getEntryBlock(), F->getEntryBlock().begin());
    auto LLVMType = getLLVMType(VarType);
    if (auto Err = LLVMType.takeError())
        return Err;
    auto *Alloca = B.CreateAlloca(*LLVMType, nullptr, VarName);
    createZeroInitializer(B, Alloca);
    NamedValues[VarName] = std::pair(VarType, Alloca);
    return llvm::Error::success();
}


llvm::Expected<llvm::Type*> codegen::Codegen::getLLVMType(ast::Type *T) {
    if (llvm::isa<ast::Integer>(T))
        return llvm::Type::getInt64Ty(Module.getContext());
    if (llvm::isa<ast::Void>(T))
        return llvm::Type::getVoidTy(Module.getContext());
    if (auto *Arr = llvm::dyn_cast<ast::Array>(T)) {
        auto Err = addConstantBoundsToArray(Arr);
        if (Err)
            return Err;
        llvm::Expected<llvm::Type*> ElemType = getLLVMType(Arr->ElemType);
        if (!ElemType)
            return ElemType;
        return llvm::ArrayType::get(ElemType.get(), (uint64_t) Arr->size());
    }
    llvm_unreachable("Unknown type converted to LLVM representation");
}

llvm::Value *codegen::Codegen::createZeroInitializer(llvm::IRBuilder<> &B, llvm::AllocaInst *Alloca) {
    auto *Zero = llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 0, true));
    if (Alloca->getType()->getElementType()->isIntegerTy(64))
        return B.CreateStore(Zero, Alloca);
    if (auto *Arr = llvm::dyn_cast<llvm::ArrayType>(Alloca->getType()->getElementType())) {
        /*llvm::Value *FetchedArr = B.CreateLoad(Alloca, "arrtoinit");
        size_t Len = Arr->getNumElements();
        std::vector<unsigned> Indices;
        Indices.reserve(Len);
        return B.CreateInsertValue(FetchedArr, Zero, llvm::ArrayRef(Indices));*/
        size_t Len = Arr->getNumElements();
        for (size_t i = 0; i < Len; i++) {
            auto *Ptr = B.CreateConstGEP2_32(Arr, Alloca, 0, i, "elemptr");
            B.CreateStore(Zero, Ptr);
        }
        return Alloca;
    }
    llvm_unreachable("Generating zero-initializer for an unsupported type");
}

llvm::Error codegen::Codegen::addConstantBoundsToArray(ast::Array *Arr) {
    if (auto Lower = llvm::dyn_cast<ast::IdentifierArrayBound>(Arr->From)) {
        auto Err = setConstantBoundValue(Lower);
        if (Err)
            return Err;
    }
    if (auto Upper = llvm::dyn_cast<ast::IdentifierArrayBound>(Arr->To)) {
        auto Err = setConstantBoundValue(Upper);
        if (Err)
            return Err;
    }
    return llvm::Error::success();
}

llvm::Error codegen::Codegen::setConstantBoundValue(ast::IdentifierArrayBound *B) {
    if (Constants.count(B->Ident) == 0)
        return llvm::make_error<llvm::StringError>("Array bound using undefined constant", llvm::inconvertibleErrorCode());
    B->setValue((int) llvm::cast<llvm::ConstantInt>(Constants[B->Ident])->getLimitedValue());
    return llvm::Error::success();
}
