//
// Created by selvek on 15.5.2018.
//
#include <llvm/ADT/APSInt.h>
#include "../ast/expr/NumExpr.h"
#include "../ast/expr/VarExpr.h"
#include "../ast/expr/UnaryOpExpr.h"
#include "../ast/expr/BinaryOpExpr.h"
#include "../ast/expr/CallExpr.h"
#include "Codegen.h"
#include "../ast/expr/ArrayIdxExpr.h"
#include "../ast/type/Integer.h"

void codegen::Codegen::visit(ast::BinaryOpExpr &E) {
    E.L->accept(*this);
    auto LHS = LastValue;
    E.R->accept(*this);
    auto RHS = LastValue;
    if (LHS == nullptr || RHS == nullptr) {
        LastValue = nullptr;
        return;
    }
    if (!LHS->getType()->isIntegerTy()) {
        return LogError(E.L->Loc, "Invalid expression type in binary operation");
    }
    if (!RHS->getType()->isIntegerTy()) {
        return LogError(E.R->Loc, "Invalid expression type in binary operation");
    }
    switch (E.Op) {
        case Lexeme::Kind::LT:
            generateICmp(llvm::CmpInst::Predicate::ICMP_SLT, LHS, RHS);
            break;
        case Lexeme::Kind::LTE:
            generateICmp(llvm::CmpInst::Predicate::ICMP_SLE, LHS, RHS);
            break;
        case Lexeme::Kind::GT:
            generateICmp(llvm::CmpInst::Predicate::ICMP_SGT, LHS, RHS);
            break;
        case Lexeme::Kind::GTE:
            generateICmp(llvm::CmpInst::Predicate::ICMP_SGE, LHS, RHS);
            break;
        case Lexeme::Kind::EQ:
            generateICmp(llvm::CmpInst::Predicate::ICMP_EQ, LHS, RHS);
            break;
        case Lexeme::Kind::NEQ:
            generateICmp(llvm::CmpInst::Predicate::ICMP_NE, LHS, RHS);
            break;
        case Lexeme::Kind::PLUS:
            LastValue = Builder.CreateAdd(LHS, RHS, "plustmp");
            break;
        case Lexeme::Kind::MINUS:
            LastValue = Builder.CreateSub(LHS, RHS, "subtmp");
            break;
        case Lexeme::Kind::MOD:
            LastValue = Builder.CreateSRem(LHS, RHS, "modtmp");
            break;
        case Lexeme::Kind::MUL:
            LastValue = Builder.CreateMul(LHS, RHS, "multmp");
            break;
        case Lexeme::Kind::DIV:
            LastValue = Builder.CreateSDiv(LHS, RHS, "divtmp");
            break;
        case Lexeme::Kind::AND:
        case Lexeme::Kind::OR:
            generateLogical(E.Op, LHS, RHS);
            break;
        default:
            llvm_unreachable("Invalid binary operation in codegen");
    }
}

void codegen::Codegen::visit(ast::CallExpr &E) {
    //TODO: Fail on trying to call a void function in an expression
    callFn(E);
}

void codegen::Codegen::visit(ast::NumExpr &E) {
    LastValue = llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, (long) E.Value, true));
}

void codegen::Codegen::visit(ast::UnaryOpExpr &E) {
    E.Expression->accept(*this);
    auto Zero = llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 0, true));
    switch (E.Op) {
        case Lexeme::Kind::MINUS:
            LastValue = Builder.CreateSub(Zero, LastValue, "unarymintmp");
            break;
        case Lexeme::Kind::NOT:
            LastValue = Builder.CreateIntCast(Builder.CreateICmpEQ(Zero, LastValue, "nottmp"),
                                              llvm::Type::getInt64Ty(Module.getContext()), true);
            break;
        default:
            LogError(E.Loc, "Invalid binary operation");
    }
}

void codegen::Codegen::visit(ast::VarExpr &E) {
    std::pair<ast::Type *, llvm::Value*> ValInfo;
    if (NamedValues.count(E.VarName)) {
        ValInfo = NamedValues[E.VarName];
    } else if (Globals.count(E.VarName)) {
        ValInfo = Globals[E.VarName];
    } else if (Constants.count(E.VarName)) {
        LastValue = Constants[E.VarName];
        return;
    } else {
        return LogError(E.Loc, "Accessing an undeclared variable");
    }

    if (ValInfo.first != ast::Integer::get()) {
        return LogError(E.Loc, "Trying to treat array as a value");
    }

    LastValue = Builder.CreateLoad(ValInfo.second);
}

void codegen::Codegen::generateICmp(llvm::CmpInst::Predicate Pred, llvm::Value *LHS, llvm::Value *RHS) {
    LHS = Builder.CreateICmp(Pred, LHS, RHS, "cmptmp");
    LastValue = Builder.CreateIntCast(LHS, llvm::Type::getInt64Ty(Module.getContext()), true);
}

void codegen::Codegen::generateLogical(Lexeme::Kind Op, llvm::Value *LHS, llvm::Value *RHS) {
    auto Zero = llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 0, true));
    LHS = Builder.CreateIntCast(Builder.CreateICmpNE(Zero, LHS, "lhscoercion"),
                                llvm::Type::getInt64Ty(Module.getContext()), true);
    RHS = Builder.CreateIntCast(Builder.CreateICmpNE(Zero, RHS, "rhscoercion"),
                                llvm::Type::getInt64Ty(Module.getContext()), true);
    if (Op == Lexeme::Kind::AND)
        LastValue = Builder.CreateAnd(LHS, RHS, "andop");
    else if (Op == Lexeme::Kind::OR)
        LastValue = Builder.CreateOr(LHS, RHS, "orop");
    else
        llvm_unreachable("Invalid logical operation in codegen");
}

void codegen::Codegen::lookupFunction(llvm::StringRef Name) {
    if (auto *F = Module.getFunction(Name)) {
        LastValue = F;
    } else if (Prototypes.count(Name)) {
        generatePrototype(*Prototypes[Name]);
        Prototypes.erase(Name);
    } else {
        LastValue = nullptr;
    }
}

void codegen::Codegen::callFn(ast::CallExpr &C) {
    lookupFunction(C.FunctionName);
    if (LastValue == nullptr)
        return LogError(C.Loc, "Undefined function called");
    auto F = llvm::dyn_cast<llvm::Function>(LastValue);
    if (F->arg_size() != C.Args.size())
        return LogError(C.Loc, "Invalid number of function arguments");
    std::vector<llvm::Value *> FunctionArgs;
    for (auto &Arg : C.Args) {
        Arg->accept(*this);
        if (LastValue == nullptr)
            return;
        FunctionArgs.push_back(LastValue);
    }
    if (F->getReturnType() == llvm::Type::getVoidTy(Module.getContext()))
        LastValue = Builder.CreateCall(F, FunctionArgs);
    else
        LastValue = Builder.CreateCall(F, FunctionArgs, "calltmp");
}


void codegen::Codegen::visit(ArrayIdxExpr &E) {
    std::pair<ast::Type *, llvm::Value*> ValInfo;
    if (NamedValues.count(E.VarName)) {
        ValInfo = NamedValues[E.VarName];
    } else if (Globals.count(E.VarName)) {
        ValInfo = Globals[E.VarName];
    } else {
        return LogError(E.Loc, "Accessing an undeclared variable");
    }

    if (!llvm::isa<ast::Array>(ValInfo.first)) {
        return LogError(E.Loc, "Trying to index a value as an array");
    }

    auto ArrStart = llvm::cast<ast::Array>(ValInfo.first)->From->value();
    auto ArrStartConstant = llvm::ConstantInt::get(Module.getContext(), llvm::APSInt::get(ArrStart));
    //TODO: Add type checking
    E.Idx->accept(*this);
    if (!LastValue)
        return;
    auto *Idx = Builder.CreateSub(LastValue, ArrStartConstant);
    llvm::Value *Idxs[] = {
            llvm::ConstantInt::get(llvm::Type::getInt64Ty(Module.getContext()), 0),
            Idx
    };
    auto *ElemPtr = Builder.CreateGEP(ValInfo.second, Idxs,"arridx");
    LastValue = Builder.CreateLoad(ElemPtr, "arridxld");
}
