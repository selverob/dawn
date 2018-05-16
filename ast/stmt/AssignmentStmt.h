//
// Created by selvek on 6.5.2018.
//

#ifndef DAWN_ASSIGNMENTSTMT_H
#define DAWN_ASSIGNMENTSTMT_H

#include <string>
#include <memory>
#include "Stmt.h"
#include "../expr/Lvalue.h"
#include "../expr/Expr.h"
#include "../Visitor.h"

namespace ast {
    class AssignmentStmt : public Stmt {
    public:
        std::unique_ptr<Lvalue> Lval;
        std::unique_ptr<Expr> Value;

        AssignmentStmt(llvm::SMLoc Loc, std::unique_ptr<Lvalue> Lval, std::unique_ptr<Expr> Value) :
                Stmt(Loc), Lval(std::move(Lval)), Value(std::move(Value)) {}

        virtual void accept(Visitor &V) {
            V.visit(*this);
        }
    };
}


#endif //DAWN_ASSIGNMENTSTMT_H
