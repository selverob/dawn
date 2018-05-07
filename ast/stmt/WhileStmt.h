//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_WHILESTMT_H
#define DAWN_WHILESTMT_H

#include <memory>
#include "Stmt.h"
#include "../Visitor.h"
#include "../expr/Expr.h"

namespace ast {
    class WhileStmt : public Stmt {
    public:
        std::unique_ptr<Expr> Condition;
        std::unique_ptr<Stmt> Body;

        WhileStmt(llvm::SMLoc Loc, std::unique_ptr<Expr> Condition, std::unique_ptr<Stmt> Body) : Stmt(Loc),
            Condition(std::move(Condition)), Body(std::move(Body)) {}

        void accept(Visitor &V) override {
            V.visit(*this);
        }
    };
}


#endif //DAWN_WHILESTMT_H
