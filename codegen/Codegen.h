//
// Created by selvek on 8.5.2018.
//

#ifndef DAWN_CODEGEN_H
#define DAWN_CODEGEN_H

#include <llvm/Support/SMLoc.h>
#include <llvm/Support/SourceMgr.h>
#include <llvm/Support/Error.h>
#include "llvm/IR/LLVMContext.h"
#include "llvm/IR/Module.h"
#include "llvm/IR/IRBuilder.h"
#include "llvm/IR/LegacyPassManager.h"
#include "llvm/ADT/StringMap.h"
#include "../ast/Visitor.h"
#include "../lex/Lexeme.h"
#include "../ast/decl/Function.h"
#include "../ast/type/ArrayBound.h"
#include "../ast/type/Array.h"
#include "../ast/type/IdentifierArrayBound.h"

using namespace ast;

namespace codegen {
    class Codegen : public Visitor {
        llvm::IRBuilder<> Builder;
        llvm::Module &Module;
        llvm::Value *LastValue;
        std::unique_ptr<llvm::legacy::FunctionPassManager> FPM;
        llvm::StringMap<llvm::AllocaInst*> NamedValues;
        llvm::StringMap<ast::Prototype*> Prototypes;
        llvm::StringMap<llvm::Constant*> Constants;

        llvm::SourceMgr &Sources;

        void LogError(llvm::SMLoc Loc, llvm::StringRef Msg) {
            Sources.PrintMessage(Loc, llvm::SourceMgr::DiagKind::DK_Error, Msg);
            LastValue =  nullptr;
            Finished = false;
        }

        llvm::AllocaInst *createAlloca(llvm::Function *F, llvm::StringRef VarName, llvm::Type *VarType);
        void lookupFunction(llvm::StringRef Name);

        void generatePrototype(Prototype& P);
        void generateICmp(llvm::CmpInst::Predicate Pred, llvm::Value *LHS, llvm::Value *RHS);
        void generateLogical(Lexeme::Kind Op, llvm::Value *LHS, llvm::Value *RHS);
        void callFn(ast::CallExpr &C);

        llvm::Type *getLLVMType(llvm::SMLoc Loc, ast::Type *T);
        llvm::Value *createZeroInitializer(llvm::IRBuilder<> &B, llvm::AllocaInst *Alloca);

        llvm::Error addConstantBoundsToArray(ast::Array *Arr);
        llvm::Error setConstantBoundValue(ast::IdentifierArrayBound *Arr);

        bool Finished;

    public:
        Codegen(llvm::SourceMgr &Sources, llvm::Module &Module, llvm::StringMap<ast::Prototype*> StdProtos);

        void visit(BinaryOpExpr &E) override;

        void visit(CallExpr &E) override;

        void visit(NumExpr &E) override;

        void visit(UnaryOpExpr &E) override;

        void visit(VarExpr &E) override;

        void visit(AssignmentStmt &E) override;

        void visit(CallStmt &E) override;

        void visit(CompoundStmt &E) override;

        void visit(ExitStmt &E) override;

        void visit(ForStmt &E) override;

        void visit(IfStmt &E) override;

        void visit(WhileStmt &E) override;

        void visit(Consts &E) override;

        void visit(Prototype &E) override;

        void visit(Function &E) override;

        void visit(Program &E) override;

        void visit(Vars &E) override;

        bool finishedSuccesfully() const;
    };
}


#endif //DAWN_CODEGEN_H
