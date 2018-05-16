//
// Created by selvek on 15.5.2018.
//

#include "Codegen.h"
#include "../ast/stmt/CallStmt.h"
#include "../ast/stmt/AssignmentStmt.h"
#include "../ast/stmt/CompoundStmt.h"
#include "../ast/stmt/IfStmt.h"
#include "../ast/stmt/WhileStmt.h"
#include "../ast/stmt/ForStmt.h"

void codegen::Codegen::visit(ast::AssignmentStmt &E) {
    auto Ptr = getLvalueAddress(E.Lval.get());
    if (!Ptr)
        return LogError(E.Loc, "Trying to assign to an undeclared variable");
    E.Value->accept(*this);
    if (!LastValue)
        return;
    LastValue = Builder.CreateStore(LastValue, Ptr);
}

void codegen::Codegen::visit(ast::CallStmt &E) {
    E.Callee->accept(*this);
}

void codegen::Codegen::visit(ast::CompoundStmt &E) {
    for (auto &Stmt : E.Body) {
        Stmt->accept(*this);
        if (LastValue == nullptr)
            return;
    }
}

void codegen::Codegen::visit(ast::ExitStmt &E) {
    auto Parent = Builder.GetInsertBlock()->getParent();
    llvm::BasicBlock *RetBlock = nullptr;
    for (auto &Block : Parent->getBasicBlockList()) {
        if (Block.getName() == "return") {
            RetBlock = &Block;
            break;
        }
    }
    assert(RetBlock != nullptr);
    Builder.CreateBr(RetBlock);

    auto NewBB = llvm::BasicBlock::Create(Module.getContext(), "afterexit", Parent);
    Builder.SetInsertPoint(NewBB);
}

void codegen::Codegen::visit(ast::ForStmt &E) {
    auto IterVar = NamedValues[E.Var];
    if (!IterVar)
        return LogError(E.Loc, "Iteration variable was not declared");
    E.Begin->accept(*this);
    if (LastValue == nullptr)
        return;
    Builder.CreateStore(LastValue, IterVar);
    E.End->accept(*this);
    if (LastValue == nullptr)
        return;
    auto FinalVal = LastValue;
    auto Fn = Builder.GetInsertBlock()->getParent();
    auto IterBB = llvm::BasicBlock::Create(Module.getContext(), "forcond", Fn);
    auto BodyBB = llvm::BasicBlock::Create(Module.getContext(), "forbody", Fn);
    auto AfterBB = llvm::BasicBlock::Create(Module.getContext(), "forafter", Fn);
    Builder.CreateBr(IterBB);
    Builder.SetInsertPoint(IterBB);
    llvm::Value *Condition;
    if (E.Operator == Lexeme::Kind::TO) {
        Condition = Builder.CreateICmpSLT(Builder.CreateLoad(IterVar, "iterationvar"), FinalVal);
    } else if (E.Operator == Lexeme::Kind::DOWNTO) {
        Condition = Builder.CreateICmpSGT(Builder.CreateLoad(IterVar, "iterationvar"), FinalVal);
    } else {
        llvm_unreachable("Invalid operand in for");
    }
    Builder.CreateCondBr(Condition, BodyBB, AfterBB);

    Builder.SetInsertPoint(BodyBB);
    E.Body->accept(*this);
    if (LastValue == nullptr)
        return;
    llvm::Value *LoadedIterVar = Builder.CreateLoad(IterVar, "iterationvar");
    llvm::Value *NewIterVarValue = nullptr;
    if (E.Operator == Lexeme::Kind::TO) {
        NewIterVarValue = Builder.CreateAdd(
                LoadedIterVar, llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 1, true)));
    } else if (E.Operator == Lexeme::Kind::DOWNTO) {
        NewIterVarValue = Builder.CreateSub(
                LoadedIterVar, llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 1, true)));
    }
    Builder.CreateStore(NewIterVarValue, IterVar);
    Builder.CreateBr(IterBB);
    Builder.SetInsertPoint(AfterBB);
}

void codegen::Codegen::visit(ast::IfStmt &E) {
    E.Condition->accept(*this);
    if (LastValue == nullptr)
        return;
    auto Condition = Builder.CreateICmpNE(LastValue,
                                          llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 0, true)),
                                          "cond");
    llvm::Function *Fn = Builder.GetInsertBlock()->getParent();
    auto ThenBB = llvm::BasicBlock::Create(Module.getContext(), "then", Fn);
    auto ElseBB = llvm::BasicBlock::Create(Module.getContext(), "else", Fn);
    auto AfterBB = llvm::BasicBlock::Create(Module.getContext(), "after", Fn);

    Builder.CreateCondBr(Condition, ThenBB, ElseBB);

    Builder.SetInsertPoint(ThenBB);
    E.IfBody->accept(*this);
    if (LastValue == nullptr)
        return;
    Builder.CreateBr(AfterBB);

    Builder.SetInsertPoint(ElseBB);
    if (E.ElseBody) {
        E.ElseBody->accept(*this);
        if (LastValue == nullptr)
            return;
    }
    Builder.CreateBr(AfterBB);

    Builder.SetInsertPoint(AfterBB);
}

void codegen::Codegen::visit(ast::WhileStmt &E) {
    auto Fn = Builder.GetInsertBlock()->getParent();
    auto CondBB = llvm::BasicBlock::Create(Module.getContext(), "whilecond", Fn);
    auto BodyBB = llvm::BasicBlock::Create(Module.getContext(), "whilebody", Fn);
    auto AfterBB = llvm::BasicBlock::Create(Module.getContext(), "whileafter", Fn);
    Builder.CreateBr(CondBB);
    Builder.SetInsertPoint(CondBB);
    E.Condition->accept(*this);
    if (LastValue == nullptr)
        return;
    auto Condition = Builder.CreateICmpNE(LastValue,
                                          llvm::ConstantInt::get(Module.getContext(), llvm::APInt(64, 0, true)),
                                          "cond");
    Builder.CreateCondBr(Condition, BodyBB, AfterBB);
    Builder.SetInsertPoint(BodyBB);
    E.Body->accept(*this);
    if (LastValue == nullptr)
        return;
    Builder.CreateBr(CondBB);
    Builder.SetInsertPoint(AfterBB);
}
