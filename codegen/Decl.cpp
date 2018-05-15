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

    llvm::StringMap<llvm::AllocaInst *> OldNamedValues;
    std::swap(NamedValues, OldNamedValues);
    for (auto &Arg : F->args()) {
        auto *Alloca = createAlloca(F, Arg.getName(), Arg.getType());
        Builder.CreateStore(&Arg, Alloca);
    }

    for (const auto &VarInfo : E.Variables->Variables) {
        auto LLVMType = getLLVMType(E.Variables->Loc, VarInfo.second);
        if (!LLVMType)
            return;
        createAlloca(F, VarInfo.first, LLVMType);
    }

    if (!llvm::isa<ast::Void>(E.Proto->ReturnType)) {
        createAlloca(F, F->getName(), F->getReturnType());
    }

    llvm::BasicBlock *RetBlock = llvm::BasicBlock::Create(Module.getContext(), "return", F);
    Builder.SetInsertPoint(RetBlock);
    if (!llvm::isa<ast::Void>(E.Proto->ReturnType)) {
        auto ReturnResult = Builder.CreateLoad(NamedValues[F->getName()]);
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
        Module.getOrInsertGlobal(Var.first, Builder.getInt64Ty());
        auto GlobalVar = Module.getNamedGlobal(Var.first);
        GlobalVar->setLinkage(llvm::GlobalValue::InternalLinkage);
        GlobalVar->setInitializer(llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 0)));
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

llvm::AllocaInst *codegen::Codegen::createAlloca(llvm::Function *F, llvm::StringRef VarName, llvm::Type *VarType) {
    llvm::IRBuilder<> B(&F->getEntryBlock(), F->getEntryBlock().begin());
    auto *Alloca = B.CreateAlloca(VarType, nullptr, VarName);
    createZeroInitializer(B, Alloca);
    NamedValues[VarName] = Alloca;
    return Alloca;
}


llvm::Type *codegen::Codegen::getLLVMType(llvm::SMLoc Loc, ast::Type *T) {
    if (llvm::isa<ast::Integer>(T))
        return llvm::Type::getInt64Ty(Module.getContext());
    if (llvm::isa<ast::Void>(T))
        return llvm::Type::getVoidTy(Module.getContext());
    if (auto *Arr = llvm::dyn_cast<ast::Array>(T)) {
        auto Err = addConstantBoundsToArray(Arr);
        if (Err) {
            LogError(Loc, "Unknown constant in array bounds");
            return nullptr;
        }
        return llvm::ArrayType::get(getLLVMType(Loc, Arr->ElemType), (uint64_t) Arr->size());
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
